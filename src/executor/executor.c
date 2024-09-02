/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wlin <wlin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/22 11:46:39 by wlin              #+#    #+#             */
/*   Updated: 2024/09/02 04:18:38 by rtorrent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	simple_command(t_data *data, t_process *process, int pipe_read_end_prev)
{
	int	fd_storage[2];

	shell_expansion(data);
	init_process(data, process, pipe_read_end_prev);
	if (process->command != NULL && process->fd_out != -1)
	{
		fd_storage[RD] = dup(STDIN_FILENO);
		fd_storage[WR] = dup(STDOUT_FILENO);
		fd_dup2(data, process->fd_in, STDIN_FILENO);
		fd_dup2(data, process->fd_out, STDOUT_FILENO);
		close(process->fd_in);
		close(process->fd_out);
		(*process->builtin)(array_len(process->command), process->command);
		fd_dup2(data, fd_storage[RD], STDIN_FILENO);
		fd_dup2(data, fd_storage[WR], STDOUT_FILENO);
		close(fd_storage[RD]);
		close(fd_storage[WR]);
	}
}

int	get_wait_status(int status)
{
	int	stat_code;

	stat_code = 0;
	if (WIFEXITED(status))
		stat_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		stat_code = WTERMSIG(status);
	else if (WIFSTOPPED(status))
		stat_code = WSTOPSIG(status);
	return (stat_code);
}

void	wait_process(pid_t *pid_array, int num_cmd)
{
	int	i;
	int	status;

	i = -1;
	while (++i < num_cmd)
	{
		waitpid(pid_array[i], &status, 0);
		get_wait_status(status);
	}
}

void	execute_command(t_data *data, char *command_path, char **cmd_args)
{
	extern char	**environ;
	char 		*shell_path;

	execve(command_path, cmd_args, environ);
	if (errno == ENOEXEC)
	{
		shell_path = getenv("SHELL");
		execve(shell_path, array_add_front(&cmd_args, shell_path), environ);
	}
	else if (errno == ENOENT && char_index(cmd_args[0], '/') == INVALID)
		exit_minishell(data, cmd_args[0], "command not found\n", NOTFOUND);
	else
		exit_minishell(data, cmd_args[0], strerror(errno), NOTFOUND);
}

void	execute_all(t_data *data)
{
	t_commands	*cmds = data->cmds;
	t_process	process;
	pid_t		*pid;
	int			i;
	const int	num_cmd = lst_size(cmds);
	int			pipe_read_end_prev;

	i = -1;
	if (cmds == NULL)
		return ;
	pipe_read_end_prev = dup(STDIN_FILENO);
	if (pipe_read_end_prev == -1)
		return ;
	if (!is_builtin(NULL, cmds->args[0]) || num_cmd > 1)
	{
		pid = malloc(sizeof(pid_t) * num_cmd);
		if (pid == NULL)
			return ;
		while (cmds)
		{
			shell_expansion(data);
			init_process(data, &process, pipe_read_end_prev);
			if (process.command != NULL && process.fd_out != -1)
				pid[++i] = create_process(data, &process);
			if (!process.builtin)
				free(process.cmd_path);
			cmds = cmds->next;
			pipe_read_end_prev = process.pipe_fd[RD];
		}
		wait_process(pid, num_cmd);
		free(pid);
	}
	else
		simple_command(data, &process, pipe_read_end_prev);
}
