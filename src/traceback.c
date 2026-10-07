/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceback.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esnavarr <esnavarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 18:42:32 by esnavarr          #+#    #+#             */
/*   Updated: 2026/10/07 18:42:33 by esnavarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
