/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:41:47 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:41:48 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "context.h"
#include "get.h"
#include "traceback.h"

static bool	is_context_correct(t_context *ctx)
{
	if (ctx->number_of_coders < 1
		|| ctx->number_of_coders > 200
		|| ctx->time_to_burnout < 1
		|| ctx->time_to_compile < 1
		|| ctx->time_to_debug < 1
		|| ctx->time_to_refactor < 1
		|| ctx->number_of_compiles_required < 1
		|| ctx->dongle_cooldown < 0
		|| ctx->scheduler == NULL)
		return (false);
	return (true);
}

static bool	init_mutexes(t_context *ctx)
{
	if (pthread_mutex_init(&ctx->ticket_mutex, NULL))
		return (false);
	if (pthread_mutex_init(&ctx->stop_mutex, NULL))
		return (pthread_mutex_destroy(&ctx->ticket_mutex), false);
	if (pthread_mutex_init(&ctx->print_mutex, NULL))
	{
		pthread_mutex_destroy(&ctx->ticket_mutex);
		pthread_mutex_destroy(&ctx->stop_mutex);
		return (false);
	}
	return (true);
}

t_context	*context_new(char const **args)
{
	t_context	*ctx;

	ctx = malloc(sizeof(t_context));
	if (!ctx)
		return (traceback(ERR_MEM, "context_new"), NULL);
	ctx->number_of_coders = atou(args[1]);
	ctx->time_to_burnout = atou(args[2]);
	ctx->time_to_compile = atou(args[3]);
	ctx->time_to_debug = atou(args[4]);
	ctx->time_to_refactor = atou(args[5]);
	ctx->number_of_compiles_required = atou(args[6]);
	ctx->dongle_cooldown = atou(args[7]);
	ctx->scheduler = get_scheduler(args[8]);
	ctx->start = 0;
	ctx->next_ticket = 0;
	ctx->stop = false;
	if (!is_context_correct(ctx))
		return (free(ctx), traceback(ERR_ARGV, "context_new"), NULL);
	if (!init_mutexes(ctx))
		return (free(ctx), traceback(ERR_MTXI, "context_new"), NULL);
	return (ctx);
}

void	context_delete(t_context *ctx)
{
	pthread_mutex_destroy(&ctx->ticket_mutex);
	pthread_mutex_destroy(&ctx->stop_mutex);
	pthread_mutex_destroy(&ctx->print_mutex);
	free(ctx);
}

bool	is_stopped(t_context *ctx)
{
	bool	result;

	pthread_mutex_lock(&ctx->stop_mutex);
	result = ctx->stop;
	pthread_mutex_unlock(&ctx->stop_mutex);
	return (result);
}
