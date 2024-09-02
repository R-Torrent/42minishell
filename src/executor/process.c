/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlin <wlin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 18:16:01 by wlin              #+#    #+#             */
/*   Updated: 2024/09/02 04:35:43 by rtorrent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	fd_dup2(t_data *data, int oldfd, int newfd)
{
	if (oldfd != -1 && dup2(oldfd, newfd) == -1)
	{
		close(oldfd);
		exit_minishell(data, "dup2", strerror(errno), errno);
	}
	close(oldfd);
}

void	child_process(t_data *data, t_process *process)
{
	char *const		cmd_path = process->cmd_path;
	char **const	command = process->command;

	if (access(cmd_path, X_OK) == -1 && access(cmd_path, F_OK) == 0)
		exit_minishell(data, command[0], "Permission denied\n", NOTEXECUTABLE);
	fd_dup2(data, process->fd_in, STDIN_FILENO);
	close(process->fd_in);
	fd_dup2(data, process->fd_out, STDOUT_FILENO);
	close(process->fd_out);
	close(process->pipe_fd[RD]);
	if (process->builtin)
		exit((*process->builtin)(array_len(command), command));
	execute_command(data, cmd_path, command);
}

pid_t	create_process(t_data *data, t_process *process)
{
	pid_t	pid;

	pid = fork();
	if (pid == INVALID)
		exit_minishell(data, "fork", strerror(errno), errno);
	else if (pid == CHILD)
		child_process(data, process);
	close(process->fd_in);
	close(process->fd_out);
	return (pid);
}
