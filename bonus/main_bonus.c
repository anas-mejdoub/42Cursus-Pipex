/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amejdoub <amejdoub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 17:32:23 by amejdoub          #+#    #+#             */
/*   Updated: 2024/03/26 17:30:10 by amejdoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

void	parent_helper(t_variables var, int argc, int *status)
{
	int	i;

	if (var.j == 3)
		close_fd(var.fd, argc - 5);
	else
		close_fd(var.fd, argc - 4);
	i = 0;
	while (i < argc - (var.j) + 1)
	{
		waitpid(var.pids[i], status, 0);
		i++;
	}
}

int	mini_helper(t_commands_ *command, t_variables *var, char *argv[],
		char *envp[])
{
	free2d((void **)command->command_args);
	free(command->path);
	command->command_args = NULL;
	command->path = NULL;
	command->command_args = option_split((char *)argv[var->i]);
	command->path = find_path(command->command_args[0], get_env(envp));
	var->pid = fork();
	if (var->pid == -1)
	{
		perror("FORK ");
		return (-1);
	}
	return (1);
}

int	helper_function(t_variables var, int argc, char *argv[], char *envp[])
{
	int			status;
	t_commands_	command;

	var.i = 2;
	if (var.is_herdoc)
		var.i = 3;
	var.j = var.i;
	status = 0;
	command.command_args = NULL;
	command.path = NULL;
	while (var.i < argc - 1)
	{
		mini_helper(&command, &var, argv, envp);
		if (var.pid == 0)
			pipes_level(argc, var, command, envp);
		else
			var.pids[var.i - var.j] = var.pid;
		var.i++;
	}
	if (var.pid > 0)
		parent_helper(var, argc, &status);
	return (free2d((void **)command.command_args), free(command.path), status);
}

void	here_doc_handler(t_variables *var, int argc, char **argv)
{
	char	*str;

	var->j = 5;
	var->is_herdoc = 1;
	var->fdin = open("/tmp/pipex.txt", O_RDWR | O_TRUNC | O_CREAT, 0777);
	var->fdout = open(argv[argc - 1], O_RDWR | O_CREAT | O_APPEND, 0777);
	while (1)
	{
		ft_printf("heredoc>");
		str = get_next_line(0);
		if (!ft_strncmp(argv[2], str, ft_strlen(str) - 1))
		{
			close(var->fdin);
			var->fdin = open("/tmp/pipex.txt", O_RDWR | O_CREAT, 0777);
			free(str);
			break ;
		}
		write(var->fdin, str, ft_strlen(str));
		free(str);
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	int			status;
	t_variables	var;

	status = 0;
	if (argc > 4)
	{
		var.is_herdoc = 0;
		var.j = 4;
		if (!ft_strncmp(argv[1], "here_doc", 8))
			here_doc_handler(&var, argc, argv);
		else
			open_helper(&var, argv, argc);
		if (var.fdout == -1 || var.fdin == -1)
			exit_error("FILE", 1);
		var.fd = malloc_2d(var.fd, argc - 4);
		var.pids = malloc_int_arr(var.pids, argc - 3);
		smart_pipe(var.fd, argc - var.j);
		status = helper_function(var, argc, argv, envp);
		unlink("/tmp/pipex.txt");
	}
	else
		ft_printf("the number of argument is not valid !");
	return (WEXITSTATUS(status));
}
