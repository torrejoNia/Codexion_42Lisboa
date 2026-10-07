/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:42:23 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:42:24 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "setup.h"
#include "traceback.h"

t_dongle	**setup_dongles(int count, t_cmp scheduler)
{
	t_dongle	**dongles;
	int			i;

	dongles = malloc(count * sizeof(t_dongle *));
	if (!dongles)
		return (traceback(ERR_MEM, "setup_dongles"), NULL);
	i = 0;
	while (i < count)
	{
		dongles[i] = dongle_new(scheduler);
		if (!dongles[i])
			return (dongles_delete(dongles, i), NULL);
		++i;
	}
	return (dongles);
}

t_coder	**setup_coders(t_context *ctx, t_dongle **dongles, int count)
{
	t_coder	**coders;
	int		left;
	int		right;

	coders = malloc(count * sizeof(t_coder *));
	if (!coders)
		return (traceback(ERR_MEM, "setup_coders"), NULL);
	left = 0;
	while (left < count)
	{
		right = (left + 1) % count;
		if (left < right)
			coders[left] = coder_new(left + 1, ctx,
					dongles[left], dongles[right]);
		else
			coders[left] = coder_new(left + 1, ctx,
					dongles[right], dongles[left]);
		if (!coders[left])
			return (coders_delete(coders, left), NULL);
		++left;
	}
	return (coders);
}

void	dongles_delete(t_dongle **dongles, int count)
{
	while (count-- > 0)
		dongle_delete(dongles[count]);
	free(dongles);
}

void	coders_delete(t_coder **coders, int count)
{
	while (count-- > 0)
		coder_delete(coders[count]);
	free(coders);
}
