/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:41:51 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:41:52 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include "dongle.h"
#include "time_utils.h"
#include "traceback.h"

t_dongle	*dongle_new(t_cmp scheduler)
{
	t_dongle	*dongle;

	dongle = malloc(sizeof(t_dongle));
	if (!dongle)
		return (traceback(ERR_MEM, "dongle_new"), NULL);
	if (pthread_mutex_init(&dongle->mutex, NULL))
		return (free(dongle), traceback(ERR_MTXI, "dongle_new"), NULL);
	if (pthread_cond_init(&dongle->cond, NULL))
	{
		pthread_mutex_destroy(&dongle->mutex);
		return (free(dongle), traceback(ERR_CNDI, "dongle_new"), NULL);
	}
	dongle->queue = pqueue_new(DONGLE_QUEUE_SIZE, scheduler);
	if (!dongle->queue)
	{
		pthread_cond_destroy(&dongle->cond);
		pthread_mutex_destroy(&dongle->mutex);
		return (free(dongle), traceback(ERR, "dongle_new"), NULL);
	}
	dongle->available_at = 0;
	dongle->in_use = false;
	return (dongle);
}

void	dongle_delete(t_dongle *dongle)
{
	pthread_cond_destroy(&dongle->cond);
	pthread_mutex_destroy(&dongle->mutex);
	pqueue_delete(dongle->queue);
	free(dongle);
}

bool	dongle_is_ready(t_dongle *dongle, int id)
{
	t_request	*first;

	first = pqueue_peek(dongle->queue);
	return (first && first->id == id && !dongle->in_use
		&& now_us() >= dongle->available_at);
}

void	dongle_wait(t_dongle *dongle, int id)
{
	t_request	*first;
	int64_t		left;

	first = pqueue_peek(dongle->queue);
	if (first && first->id == id && !dongle->in_use)
	{
		left = dongle->available_at - now_us();
		if (left > SLEEP_SLICE_US)
			left = SLEEP_SLICE_US;
		pthread_mutex_unlock(&dongle->mutex);
		if (left > 0)
			usleep(left);
		pthread_mutex_lock(&dongle->mutex);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

void	drop_dongle(t_dongle *dongle, int cooldown)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = false;
	dongle->available_at = now_us() + cooldown * 1000LL;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
