
#include "cmp.h"

static int	compare(int64_t a, int64_t b)
{
	return ((a > b) - (a < b));
}

int	cmp_fifo(t_request const *a, t_request const *b)
{
	return (compare(a->ticket, b->ticket));
}

int	cmp_edf(t_request const *a, t_request const *b)
{
	if (a->deadline != b->deadline)
		return (compare(a->deadline, b->deadline));
	return (compare(a->ticket, b->ticket));
}
