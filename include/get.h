
#ifndef GET_H
# define GET_H

# include "cmp.h"

/**
 * @brief Convert a string of digits to a non-negative integer.
 *
 * @param s String to convert.
 * @return Converted integer, or -1 if @p s is empty, contains a character
 *         that is not a digit, or does not fit in an `int`.
 */
int		atou(char const *s);

/**
 * @brief Get the scheduler for a scheduler name.
 *
 * @param s Name of the scheduler.
 * @return Comparator of the scheduler, or `NULL` if the name is unknown.
 */
t_cmp	get_scheduler(char const *s);

#endif
