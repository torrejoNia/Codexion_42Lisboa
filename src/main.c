
#include "context.h"
#include "coder.h"
#include "setup.h"
#include "monitor.h"
#include "log_state.h"
#include "time_utils.h"
#include "traceback.h"

/*
** Start the clock and one thread per coder.
** Return the number of threads that were successfully created.
*/
static int	start_coders(t_context *ctx, t_coder **coders)
{
	int	i;

	ctx->start = now_us();
	i = 0;
	while (i < ctx->number_of_coders)
		coders[i++]->last_compile = ctx->start;
	i = 0;
	while (i < ctx->number_of_coders
		&& !pthread_create(&coders[i]->thread, NULL, &coder_routine, coders[i]))
		++i;
	return (i);
}

/*
** Start the coders, then the monitor. If a thread cannot be created,
** the simulation is stopped and the started coders are joined.
*/
static int	run_threads(t_context *ctx, t_coder **coders)
{
	pthread_t	monitor;
	int			created;
	bool		failed;

	created = start_coders(ctx, coders);
	failed = (created < ctx->number_of_coders
			|| pthread_create(&monitor, NULL, &monitor_routine, coders));
	if (failed)
	{
		stop_simulation(ctx, 0);
		wake_coders(coders, created);
	}
	else
		pthread_join(monitor, NULL);
	while (created-- > 0)
		pthread_join(coders[created]->thread, NULL);
	if (failed)
		return (traceback(ERR_THRC, "run_threads"));
	return (0);
}

int	main(int argc, char const *argv[])
{
	t_context	*ctx;
	t_dongle	**dongles;
	t_coder		**coders;
	int			status;

	if (argc != 9)
		return (traceback(ERR_ARGC, "main"));
	ctx = context_new(argv);
	if (!ctx)
		return (1);
	dongles = setup_dongles(ctx->number_of_coders, ctx->scheduler);
	if (!dongles)
		return (context_delete(ctx), 1);
	coders = setup_coders(ctx, dongles, ctx->number_of_coders);
	if (!coders)
		return (dongles_delete(dongles, ctx->number_of_coders),
			context_delete(ctx), 1);
	status = run_threads(ctx, coders);
	coders_delete(coders, ctx->number_of_coders);
	dongles_delete(dongles, ctx->number_of_coders);
	context_delete(ctx);
	return (status);
}
