
#include "coder_routine_utils.h"

/* Dongles are always locked in the same order (first, then second). */
static void	lock_pair(t_coder *coder, bool lock)
{
	if (lock)
	{
		pthread_mutex_lock(&coder->first->mutex);
		pthread_mutex_lock(&coder->second->mutex);
		return ;
	}
	pthread_mutex_unlock(&coder->second->mutex);
	pthread_mutex_unlock(&coder->first->mutex);
}

/* Give every request a unique arrival number, shared by all dongles. */
static int64_t	take_ticket(t_context *ctx)
{
	int64_t	ticket;

	pthread_mutex_lock(&ctx->ticket_mutex);
	ticket = ctx->next_ticket;
	ctx->next_ticket += 1;
	pthread_mutex_unlock(&ctx->ticket_mutex);
	return (ticket);
}

/* Return the dongle the coder is waiting for, or NULL if both are ready. */
static t_dongle	*blocking_dongle(t_coder *coder)
{
	if (!dongle_is_ready(coder->first, coder->id))
		return (coder->first);
	if (!dongle_is_ready(coder->second, coder->id))
		return (coder->second);
	return (NULL);
}

/*
** Called and returns with both dongle mutexes locked. While waiting, only
** the mutex of the dongle that blocks the coder stays locked.
*/
static bool	wait_for_turn(t_coder *coder)
{
	t_dongle	*blocking;

	blocking = blocking_dongle(coder);
	while (blocking && !is_stopped(coder->ctx))
	{
		if (blocking == coder->first)
			pthread_mutex_unlock(&coder->second->mutex);
		else
			pthread_mutex_unlock(&coder->first->mutex);
		dongle_wait(blocking, coder->id);
		pthread_mutex_unlock(&blocking->mutex);
		lock_pair(coder, true);
		blocking = blocking_dongle(coder);
	}
	return (blocking == NULL);
}

int	take_dongles(t_coder *coder)
{
	t_request	request;
	bool		granted;

	request.id = coder->id;
	request.deadline = coder->last_compile
		+ coder->ctx->time_to_burnout * 1000LL;
	lock_pair(coder, true);
	request.ticket = take_ticket(coder->ctx);
	pqueue_push(coder->first->queue, request);
	pqueue_push(coder->second->queue, request);
	granted = wait_for_turn(coder);
	if (granted)
	{
		pqueue_pop(coder->first->queue, NULL);
		pqueue_pop(coder->second->queue, NULL);
		coder->first->in_use = true;
		coder->second->in_use = true;
	}
	lock_pair(coder, false);
	return (!granted);
}
