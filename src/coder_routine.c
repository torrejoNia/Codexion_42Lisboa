
#include <unistd.h>
#include "coder_routine_utils.h"
#include "time_utils.h"
#include "log_state.h"

static void	drop_both(t_coder *coder, int cooldown)
{
	drop_dongle(coder->first, cooldown);
	drop_dongle(coder->second, cooldown);
}

static int	take_both(t_coder *coder)
{
	if (take_dongles(coder))
		return (1);
	if (!log_state(coder->ctx, coder->id, LOG_DONGLE)
		|| !log_state(coder->ctx, coder->id, LOG_DONGLE))
		return (drop_both(coder, 0), 1);
	return (0);
}

/*
** "is debugging" is printed before the dongles are dropped, so a neighbour
** can never print "has taken a dongle" before this coder has let go of it.
*/
static int	work(t_coder *coder)
{
	t_context	*ctx;
	bool		logged;

	ctx = coder->ctx;
	pthread_mutex_lock(&coder->stats_mutex);
	coder->last_compile = now_us();
	pthread_mutex_unlock(&coder->stats_mutex);
	if (!log_state(ctx, coder->id, LOG_COMPILE))
		return (drop_both(coder, 0), 1);
	sim_sleep(ctx, ctx->time_to_compile);
	pthread_mutex_lock(&coder->stats_mutex);
	coder->compiles += 1;
	pthread_mutex_unlock(&coder->stats_mutex);
	logged = log_state(ctx, coder->id, LOG_DEBUG);
	drop_both(coder, ctx->dongle_cooldown);
	if (!logged)
		return (1);
	sim_sleep(ctx, ctx->time_to_debug);
	if (!log_state(ctx, coder->id, LOG_REFACTOR))
		return (1);
	sim_sleep(ctx, ctx->time_to_refactor);
	return (0);
}

/* A single coder only has one dongle: it holds it until it burns out. */
static void	lone_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->first->mutex);
	coder->first->in_use = true;
	pthread_mutex_unlock(&coder->first->mutex);
	if (log_state(coder->ctx, coder->id, LOG_DONGLE))
		while (!is_stopped(coder->ctx))
			sim_sleep(coder->ctx, coder->ctx->time_to_burnout);
	drop_dongle(coder->first, 0);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = arg;
	if (coder->first == coder->second)
		return (lone_coder(coder), NULL);
	if (coder->id % 2 == 0)
		usleep(START_OFFSET_US);
	while (!take_both(coder) && !work(coder))
		;
	return (NULL);
}
