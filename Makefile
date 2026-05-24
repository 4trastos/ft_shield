SERVER = ft_shield
CLIENT = ft_backdoor
CC = gcc 
CFLAGS = -Wall -Wextra -Werror -g3
RM = rm -f

SRC_SERVER = src/main.c
SRC_CLIENT =  src/knock_knock.c src/socket.c

OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -o $(SERVER) $(OBJ_SERVER)

$(CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $(CLIENT) $(OBJ_CLIENT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_SERVER)
	$(RM) $(OBJ_CLIENT)

fclean: clean
	$(RM) $(SERVER)
	$(RM) $(CLIENT)

re: fclean all

.PHONY: all clean fclean re