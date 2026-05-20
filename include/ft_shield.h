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
# include <pwd.h>

typedef struct s_troyan
{
    struct passwd   *pwd;
} t_troyan;

/*** MAIN ***/
int main();

#endif