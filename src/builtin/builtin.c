/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlin <wlin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/21 14:12:00 by wlin              #+#    #+#             */
/*   Updated: 2024/10/30 13:31:05 by rtorrent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(t_bfunc *dst, char *cmd)
{
	static const char		*builtin[] = {"echo", "env", "exit", "export",
		"cd", "pwd", "unset", NULL};
	static const t_bfunc	bt_func[] = {bt_echo, bt_env, bt_exit, bt_export,
		bt_cd, bt_pwd, bt_unset, NULL};
	const char				**b;

	b = builtin;
	while (*b)
	{
		if (cmd && !ft_strncmp(*b, cmd, -1))
			break ;
		b++;
	}
	if (dst)
		*dst = bt_func[b - builtin];
	return (bt_func[b - builtin] != NULL);
}
