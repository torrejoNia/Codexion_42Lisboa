
#include <stdlib.h>
#include "coder.h"
#include "traceback.h"

t_coder	*coder_new(int id, t_context *ctx, t_dongle *first, t_dongle *second)
{
	t_coder	*coder;

	coder = malloc(sizeof(t_coder));
	if (!coder)
		return (traceback(ERR_MEM, "coder_new"), NULL);
	if (pthread_mutex_init(&coder->stats_mutex, NULL))
		return (free(coder), traceback(ERR_MTXI, "coder_new"), NULL);
	coder->id = id;
	coder->compiles = 0;
	coder->last_compile = 0;
	coder->ctx = ctx;
	coder->first = first;
	coder->second = second;
	return (coder);
}

void	coder_delete(t_coder *coder)
{
	pthread_mutex_destroy(&coder->stats_mutex);
	free(coder);
}
