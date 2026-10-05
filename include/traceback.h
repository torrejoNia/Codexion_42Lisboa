
#ifndef TRACEBACK_H
# define TRACEBACK_H

# define ERR "An error occurred."
# define ERR_MEM "Failed to allocate memory."
# define ERR_ARGC "Number of arguments must be 8."
# define ERR_ARGV "Arguments are invalid (must be ints above 0 and fifo/edf)."
# define ERR_THRC "Failed to create a thread."
# define ERR_MTXI "Failed to initialize a mutex."
# define ERR_CNDI "Failed to initialize a condition variable."

/**
 * @brief Print a short error message along with its origin.
 *
 * @param msg Description of the type of error.
 * @param func Function the error originated from.
 * @return Always returns 1.
 */
int	traceback(char const *msg, char const *func);

#endif
