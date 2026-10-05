
#ifndef CONTEXT_H
# define CONTEXT_H

# include <stdbool.h>
# include <stdint.h>
# include <pthread.h>
# include "cmp.h"

/** @brief Configuration and shared state of the simulation. */
typedef struct s_context
{
	/** @brief Number of coders. Also the number of dongles. */
	int				number_of_coders;
	/**
	 * @brief Time it takes for a coder to burn out, in milliseconds.
	 *
	 * If a coder doesn't start compiling before this amount of time passes,
	 * counted from the beginning of the simulation or from the start of
	 * their last compile, they burn out and the simulation stops.
	 */
	int				time_to_burnout;
	/** @brief Duration of a compilation, in milliseconds. */
	int				time_to_compile;
	/** @brief Duration of debugging, in milliseconds. */
	int				time_to_debug;
	/** @brief Duration of refactoring, in milliseconds. */
	int				time_to_refactor;
	/** @brief Compilations required per coder before the simulation ends. */
	int				number_of_compiles_required;
	/** @brief Time a released dongle stays unavailable, in milliseconds. */
	int				dongle_cooldown;
	/** @brief Scheduling policy used to arbitrate dongle requests. */
	t_cmp			scheduler;
	/** @brief Absolute time (us) at which the simulation started. */
	int64_t			start;
	/** @brief Ticket given to the next dongle request (arrival counter). */
	int64_t			next_ticket;
	/** @brief Mutex protecting `next_ticket`. */
	pthread_mutex_t	ticket_mutex;
	/** @brief Flag that signifies the end of the simulation. */
	bool			stop;
	/** @brief Mutex protecting `stop`. */
	pthread_mutex_t	stop_mutex;
	/** @brief Mutex used to serialize console output. */
	pthread_mutex_t	print_mutex;
}	t_context;

/**
 * @brief Parse command-line arguments and create a simulation context.
 *
 * @param args Argument array, as passed to `main()`.
 * @return A newly allocated context, or `NULL` if the arguments are invalid.
 */
t_context	*context_new(char const **args);

/**
 * @brief Destroy a simulation context.
 *
 * @param ctx Context to destroy.
 */
void		context_delete(t_context *ctx);

/**
 * @brief Check whether the simulation has been told to stop.
 *
 * @param ctx Simulation context.
 * @return `true` if the simulation should stop, `false` otherwise.
 */
bool		is_stopped(t_context *ctx);

#endif
