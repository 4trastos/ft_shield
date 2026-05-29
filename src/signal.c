#define _POSIX_C_SOURCE 200809L

#include "ft_shield.h"

void    sigalrm_handler(int signum)
{
    (void)signum;
    g_sigalrm_received = 1;
}

void    sigint_handler(int signum)
{
    (void)signum;
    g_sigint_received = 1;
}

void    init_signal()
{
    struct sigaction sa_int;
    struct sigaction sa_alrm;

    memset(&sa_int, 0, sizeof(sa_int));
    memset(&sa_alrm, 0, sizeof(sa_alrm));

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_alrm.sa_handler = sigalrm_handler;
    sigemptyset(&sa_alrm.sa_mask);
    sa_alrm.sa_flags = 0;
    sigaction(SIGALRM, &sa_alrm, NULL);
}
