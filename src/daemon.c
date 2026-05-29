#include "ft_shield.h"

int	ft_lockfile(void)
{
	int		fd;
	char	pid_str[16];
	pid_t	pid;

	fd = open(PID_FILE, O_RDWR | O_CREAT, 0640);
	if (fd < 0)
		return (-1);

	if (flock(fd, LOCK_EX | LOCK_NB) < 0)
	{
		close(fd);
		return (-1);
	}

	pid = getpid();
	snprintf(pid_str, sizeof(pid_str), "%d\n", pid);
	write(fd, pid_str, strlen(pid_str));

	return (0);
}

int ft_create_daemon(t_troyan *shield)
{
    pid_t   pid;
    int     dev_null;

    (void)shield;
    pid = fork();
    if (pid < 0)
        return (-1);
    if (pid > 0)
        exit (0);
    
    if (setsid() < 0)
        return (-1);
    
    signal(SIGHUP, SIG_IGN);

    pid =fork();
    if (pid < 0)
        return (-1);
    if (pid > 0)
        exit (0);
    
    umask(0);
    chdir("/");

    dev_null = open("dev/null", O_RDWR);
    if (dev_null < 0)
        return (-1);
    dup2(dev_null, STDIN_FILENO);
    dup2(dev_null, STDOUT_FILENO);
    dup2(dev_null, STDERR_FILENO);
    close(dev_null);
    
    return (0);
}