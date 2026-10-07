/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:40:46 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:40:48 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DONGLE_H
# define DONGLE_H

# include <pthread.h>
# include <stdint.h>
# include <stdbool.h>
# include "pqueue.h"

/** @brief A dongle is shared by two coders, so at most two requests wait. */
# define DONGLE_QUEUE_SIZE 2

/** @brief Represents a single shared dongle. */
typedef struct s_dongle
{
	/** @brief Mutex protecting every field below. */
	pthread_mutex_t	mutex;
	/** @brief Condition variable used to wake waiting coders. */
	pthread_cond_t	cond;
	/** @brief Queue of coders waiting to acquire this dongle. */
	t_pqueue		*queue;
	/** @brief Absolute time (us) at which the cooldown ends. */
	int64_t			available_at;
	/** @brief Whether a coder is currently holding the dongle. */
	bool			in_use;
}	t_dongle;

/**
 * @brief Create a new dongle.
 *
 * @param scheduler Comparator used to arbitrate requests for this dongle.
 * @return A newly allocated dongle, or `NULL` on failure.
 */
t_dongle	*dongle_new(t_cmp scheduler);

/**
 * @brief Destroy a dongle.
 *
 * Frees the dongle and destroys its mutex, condition variable, and queue.
 *
 * @param dongle Dongle to destroy.
 */
void		dongle_delete(t_dongle *dongle);

/**
 * @brief Check whether a coder may take the dongle right now.
 *
 * True when the coder's request is the first one in the queue, nobody
 * holds the dongle and its cooldown is over.
 * Must be called with the dongle mutex locked.
 *
 * @param dongle Dongle to check.
 * @param id Number of the coder.
 * @return `true` if the coder may take the dongle.
 */
bool		dongle_is_ready(t_dongle *dongle, int id);

/**
 * @brief Sleep until something changes on the dongle.
 *
 * If the coder is only waiting for the cooldown, releases the mutex and
 * sleeps a short slice towards the end of the cooldown.
 * Otherwise, sleeps until another thread broadcasts on the dongle.
 * Must be called with the dongle mutex locked (and only that one);
 * returns with it locked again.
 *
 * @param dongle Dongle the coder is waiting for.
 * @param id Number of the coder.
 */
void		dongle_wait(t_dongle *dongle, int id);

/**
 * @brief Release a dongle and start its cooldown.
 *
 * Marks the dongle as free, records when the cooldown ends, and wakes
 * the waiting coders so they can re-check whether it is their turn.
 *
 * @param dongle Dongle to release.
 * @param cooldown Cooldown period in milliseconds.
 */
void		drop_dongle(t_dongle *dongle, int cooldown);

#endif
