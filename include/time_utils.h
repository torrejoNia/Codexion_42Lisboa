
#ifndef TIME_UTILS_H
# define TIME_UTILS_H

# include <stdint.h>
# include "context.h"

/** @brief Longest single nap while sleeping, in microseconds. */
# define SLEEP_SLICE_US 500

/**
 * @brief Get the current absolute time in microseconds.
 *
 * @return Time since the Epoch, in microseconds.
 */
int64_t	now_us(void);

/**
 * @brief Sleep for @p ms milliseconds, or until the simulation stops.
 *
 * Sleeps in short slices towards a fixed end time, so the delay is
 * precise and the coder notices a stop quickly.
 *
 * @param ctx Simulation context.
 * @param ms Duration to sleep, in milliseconds.
 */
void	sim_sleep(t_context *ctx, int ms);

#endif
