
#include <string.h>
#include <limits.h>
#include "get.h"

int	atou(char const *s)
{
	long	n;

	if (!*s)
		return (-1);
	n = 0;
	while (*s)
	{
		if (*s < '0' || *s > '9')
			return (-1);
		n = n * 10 + (*s - '0');
		if (n > INT_MAX)
			return (-1);
		++s;
	}
	return ((int)n);
}

t_cmp	get_scheduler(char const *s)
{
	if (!strcmp(s, "fifo"))
		return (&cmp_fifo);
	if (!strcmp(s, "edf"))
		return (&cmp_edf);
	return (NULL);
}
