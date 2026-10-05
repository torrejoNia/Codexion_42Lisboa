
#ifndef LOG_STATE_H
# define LOG_STATE_H

# include <stdbool.h>
# include "context.h"

# define LOG_DONGLE "has taken a dongle"
# define LOG_COMPILE "is compiling"
# define LOG_DEBUG "is debugging"
# define LOG_REFACTOR "is refactoring"
# define LOG_BURNOUT "burned out"

/**
 * @brief Print a timestamped state change, unless the simulation stopped.
 *
 * The stop flag is checked while holding the print mutex, so nothing can
 * be printed after the simulation was stopped.
 *
 * @param ctx Simulation context.
 * @param id Number of the coder.
 * @param msg Message describing the coder's state.
 * @return `true` if the message was printed, `false` if the simulation
 *         has stopped (the caller must then exit).
 */
bool	log_state(t_context *ctx, int id, char const *msg);

/**
 * @brief Stop the simulation, optionally announcing a burnout.
 *
 * The burnout message and the stop flag are written while holding the
 * print mutex, so the burnout message is always the last line printed.
 *
 * @param ctx Simulation context.
 * @param burned_out_id Number of the coder that burned out,
 *                      or 0 if the simulation ended normally.
 */
void	stop_simulation(t_context *ctx, int burned_out_id);

#endif
