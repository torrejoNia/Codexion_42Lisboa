
#ifndef SETUP_H
# define SETUP_H

# include "dongle.h"
# include "coder.h"
# include "context.h"

/**
 * @brief Set up an array of dongles.
 *
 * @param count Amount of dongles to create.
 * @param scheduler Comparator used to arbitrate requests for each dongle.
 * @return Array of dongles, or `NULL` on failure.
 */
t_dongle	**setup_dongles(int count, t_cmp scheduler);

/**
 * @brief Set up an array of coders.
 *
 * Coder N sits between dongle N - 1 and dongle N (modulo count).
 * Each coder takes the dongle with the lowest index first.
 *
 * @param ctx Simulation context.
 * @param dongles Array of dongles to link to the coders.
 * @param count Amount of coders to create.
 * @return Array of coders, or `NULL` on failure.
 */
t_coder		**setup_coders(t_context *ctx, t_dongle **dongles, int count);

/**
 * @brief Destroy the first @p count dongles and free the array.
 *
 * @param dongles Array of dongles.
 * @param count Number of dongles to destroy.
 */
void		dongles_delete(t_dongle **dongles, int count);

/**
 * @brief Destroy the first @p count coders and free the array.
 *
 * @param coders Array of coders.
 * @param count Number of coders to destroy.
 */
void		coders_delete(t_coder **coders, int count);

#endif
