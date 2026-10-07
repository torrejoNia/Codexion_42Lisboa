/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_utils.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:40:26 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:40:30 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_ROUTINE_UTILS_H
# define CODER_ROUTINE_UTILS_H

# include "coder.h"

/**
 * @brief Acquire both dongles of a coder through their scheduler queues.
 *
 * Pushes one request (with a single ticket) into the queue of each
 * dongle, then waits until the coder may take both of them at the same
 * time: first in both queues, both free, both cooldowns over.
 * A coder never holds one dongle while waiting for the other one.
 *
 * @param coder Coder attempting to acquire its dongles.
 * @return 0 if both dongles were acquired, 1 if the simulation stopped.
 */
int		take_dongles(t_coder *coder);

#endif
