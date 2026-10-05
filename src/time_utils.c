
#include <sys/time.h>
#include <unistd.h>
#include "time_utils.h"

int64_t	now_us(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((int64_t)tv.tv_sec * 1000000 + tv.tv_usec);
}

void	sim_sleep(t_context *ctx, int ms)
{
	int64_t	end;
	int64_t	left;

	end = now_us() + ms * 1000LL;
	left = end - now_us();
	while (left > 0 && !is_stopped(ctx))
	{
		if (left > SLEEP_SLICE_US)
			left = SLEEP_SLICE_US;
		usleep(left);
		left = end - now_us();
	}
}
