#include "../include/ft_shield.h"

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