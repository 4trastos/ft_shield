#include "../include/ft_shield.h"

int ft_shell_control(t_backdoor *backdoor)
{
    backdoor->socketfd = socket(AF_INET, SOCK_STREAM, 0);   // socket de flujo TCP convencional
    if (backdoor->socketfd == -1)
    {
        printf("Error: Could not create TCP socket.\n");
        return (-1);
    }

    memset(&backdoor->server_addr, 0, sizeof(backdoor->server_addr));
    backdoor->server_addr.sin_family = AF_INET;
    backdoor->server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, backdoor->target_ip, &backdoor->server_addr.sin_addr) <= 0)
    {
        printf("Error: Invalid target IP address.\n");
        close(backdoor->socketfd);
        return (-1);
    }

    if (connect(backdoor->socketfd, (struct sockaddr *)&backdoor->server_addr, sizeof(backdoor->server_addr)) < 0)
    {
        printf("Error: Connection to %s:%d failed.\n", backdoor->target_ip, PORT);
        close(backdoor->socketfd);
        return (-1);
    }

    printf("Connected! Entering interactive terminal mode...\n");

    // BUCLE INTERACTIVO BIDIRECCIONAL (Multiplexación con select)
    while (!g_sigint_received)
    {
        FD_ZERO(&backdoor->readfds);
        FD_SET(STDIN_FILENO, &backdoor->readfds);             // Teclado host (nuestro)
        FD_SET(backdoor->socketfd, &backdoor->readfds);       // Respuesta del Server

        // Esperamos a que ocurra actividad en cualquiera de los dos lados
        if (select(backdoor->socketfd + 1, &backdoor->readfds, NULL, NULL, NULL) < 0)
        {
            if (errno == EINTR)
                continue;
            break;;
        }

        // Caso A: Hemos escrito algo en nuestro teclado host -> Enviarlo al servidor
        if (FD_ISSET(STDIN_FILENO, &backdoor->readfds))
        {
            memset(backdoor->buff, 0, sizeof(backdoor->buff));
            backdoor->bytes_read = read(STDIN_FILENO, backdoor->buff, sizeof(backdoor->buff) - 1);
            if (backdoor->bytes_read <= 0)
                break;

            if (send(backdoor->socketfd, backdoor->buff, backdoor->bytes_read, 0) < 0)
                break;
        }

        // Caso B: El servidor nos envía texto (El prompt de auth o la respuesta de la Shell)
        if (FD_ISSET(backdoor->socketfd, &backdoor->readfds))
        {
            memset(backdoor->buff, 0, sizeof(backdoor->buff));
            backdoor->bytes_read = recv(backdoor->socketfd, backdoor->buff, sizeof(backdoor->buff) - 1, 0);
            if (backdoor->bytes_read <= 0)
            {
                printf("\nConnection closed by remote host.\n");
                break;
            }
            write(STDOUT_FILENO, backdoor->buff, backdoor->bytes_read);
        }
    }
    
    close(backdoor->socketfd);
    return (0);
}