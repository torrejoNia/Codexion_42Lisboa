
#include <stdio.h>

int	traceback(char const *msg, char const *func)
{
	fprintf(
		stderr,
		"\033[91mError in function \"%s\":\n\t%s\n\033[0m",
		func, msg
		);
	return (1);
}
