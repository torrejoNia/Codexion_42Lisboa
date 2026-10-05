
#ifndef CMP_H
# define CMP_H

# include <stdint.h>

/**
 * @brief A coder's request for a dongle, as stored in the dongle's queue.
 *
 * The request carries everything a scheduler needs to decide who goes
 * first, so comparators never have to read shared coder state.
 */
typedef struct s_request
{
	/** @brief Identifier of the coder that made the request. */
	int				id;
	/** @brief Burnout deadline of the coder, in microseconds. */
	int64_t			deadline;
	/** @brief Arrival order of the request on its dongle (0, 1, 2...). */
	int64_t			ticket;
}	t_request;

/**
 * @brief Scheduler: decides which of two requests is served first.
 *
 * @param a First request.
 * @param b Second request.
 * @return A negative value if @p a must be served before @p b,
 *         a positive value if @p b must be served before @p a.
 */
typedef int	(*t_cmp)(t_request const *a, t_request const *b);

/**
 * @brief First In, First Out: the oldest request (lowest ticket) wins.
 *
 * @param a First request.
 * @param b Second request.
 * @return A negative value if @p a arrived before @p b.
 */
int	cmp_fifo(t_request const *a, t_request const *b);

/**
 * @brief Earliest Deadline First: the request closest to burnout wins.
 *
 * Deadline = last_compile_start + time_to_burnout.
 * Equal deadlines are broken by arrival order, so the policy stays
 * fully deterministic.
 *
 * @param a First request.
 * @param b Second request.
 * @return A negative value if @p a has the earlier deadline.
 */
int	cmp_edf(t_request const *a, t_request const *b);

#endif
