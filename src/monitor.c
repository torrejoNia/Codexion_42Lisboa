
#include <unistd.h>
#include "monitor.h"
#include "time_utils.h"
#include "log_state.h"

static void	broadcast(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	wake_coders(t_coder **coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		broadcast(coders[i]->first);
		broadcast(coders[i]->second);
		++i;
	}
}

/*
** Check every coder for a burnout, then check whether all of them are done.
** Like the logs, the burnout check works in whole milliseconds: a coder
** burns out once more than time_to_burnout ms passed since its last compile.
** Return 1 (after stopping the simulation) if the simulation must end.
*/
static int	check_coders(t_coder **coders, t_context *ctx)
{
	int		i;
	int		finished;
	int64_t	last_compile;
	int		compiles;

	i = 0;
	finished = 0;
	while (i < ctx->number_of_coders)
	{
		pthread_mutex_lock(&coders[i]->stats_mutex);
		last_compile = coders[i]->last_compile;
		compiles = coders[i]->compiles;
		pthread_mutex_unlock(&coders[i]->stats_mutex);
		if ((now_us() - last_compile) / 1000 > ctx->time_to_burnout)
			return (stop_simulation(ctx, coders[i]->id), 1);
		finished += (compiles >= ctx->number_of_compiles_required);
		++i;
	}
	if (finished == ctx->number_of_coders)
		return (stop_simulation(ctx, 0), 1);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_coder		**coders;
	t_context	*ctx;

	coders = arg;
	ctx = coders[0]->ctx;
	while (!check_coders(coders, ctx))
		usleep(MONITOR_INTERVAL_US);
	wake_coders(coders, ctx->number_of_coders);
	return (NULL);
}
