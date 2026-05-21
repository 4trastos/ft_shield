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

typedef struct s_troyan
{
    struct passwd   *pwd;
    int             bytes_read;
    char            buffer_exe[1024];
    FILE            *fd;
    FILE            *fd_dest;
} t_troyan;

/*** FUNCTIONS ***/
int     main();
void    ft_payload(t_troyan *shield);

#endif