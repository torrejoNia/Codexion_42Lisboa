/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:42:00 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:42:01 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "log_state.h"
#include "time_utils.h"

bool	log_state(t_context *ctx, int id, char const *msg)
{
	bool	printed;

	pthread_mutex_lock(&ctx->print_mutex);
	printed = !is_stopped(ctx);
	if (printed)
		printf("%lld %d %s\n",
			(long long)((now_us() - ctx->start) / 1000), id, msg);
	pthread_mutex_unlock(&ctx->print_mutex);
	return (printed);
}

void	stop_simulation(t_context *ctx, int burned_out_id)
{
	pthread_mutex_lock(&ctx->print_mutex);
	if (burned_out_id && !is_stopped(ctx))
		printf("%lld %d %s\n",
			(long long)((now_us() - ctx->start) / 1000),
			burned_out_id, LOG_BURNOUT);
	pthread_mutex_lock(&ctx->stop_mutex);
	ctx->stop = true;
	pthread_mutex_unlock(&ctx->stop_mutex);
	pthread_mutex_unlock(&ctx->print_mutex);
}
