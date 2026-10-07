/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:41:01 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:41:03 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MONITOR_H
# define MONITOR_H

# include "coder.h"

/** @brief Time between two checks of the monitor, in microseconds. */
# define MONITOR_INTERVAL_US 1000

/**
 * @brief Monitor the simulation until it ends.
 *
 * Checks every coder each millisecond. Stops the simulation as soon as
 * a coder burns out or every coder has compiled enough times.
 *
 * @param arg Array of coders to monitor (`t_coder **`).
 * @return `NULL`.
 */
void	*monitor_routine(void *arg);

/**
 * @brief Wake every coder waiting on a dongle, so they can see the stop.
 *
 * @param coders Array of coders.
 * @param count Number of coders in the array.
 */
void	wake_coders(t_coder **coders, int count);

#endif
