/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:40:34 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:40:35 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

# include <pthread.h>
# include <stdint.h>
# include "context.h"
# include "dongle.h"

/** @brief Delay before even coders start, so odd coders grab first (us). */
# define START_OFFSET_US 1000

/** @brief Represents a single coder participating in the simulation. */
typedef struct s_coder
{
	/** @brief Thread executing this coder's routine. */
	pthread_t		thread;
	/** @brief Number of the coder, from 1 to number_of_coders. */
	int				id;
	/** @brief Number of completed compilations. */
	int				compiles;
	/** @brief Absolute time (us) at which the last compilation started. */
	int64_t			last_compile;
	/** @brief Mutex protecting `compiles` and `last_compile`. */
	pthread_mutex_t	stats_mutex;
	/** @brief Pointer to the shared simulation context. */
	t_context		*ctx;
	/** @brief Adjacent dongle with the lowest index: always taken first. */
	t_dongle		*first;
	/** @brief Adjacent dongle with the highest index: taken second. */
	t_dongle		*second;
}	t_coder;

/**
 * @brief Create a new coder.
 *
 * @param id Number of the coder.
 * @param ctx Simulation context.
 * @param first Dongle to take first.
 * @param second Dongle to take second.
 * @return A newly allocated coder, or `NULL` on failure.
 */
t_coder	*coder_new(int id, t_context *ctx, t_dongle *first, t_dongle *second);

/**
 * @brief Destroy a coder.
 *
 * @param coder Coder to destroy.
 */
void	coder_delete(t_coder *coder);

/**
 * @brief Coder main loop: take dongles, compile, debug, refactor, repeat.
 *
 * @param arg The coder (`t_coder *`).
 * @return `NULL`.
 */
void	*coder_routine(void *arg);

#endif
