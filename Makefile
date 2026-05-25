SERVER = ft_shield
CLIENT = ft_backdoor
CC = gcc 
CFLAGS = -Wall -Wextra -Werror -g3
RM = rm -f

VM_IP = 192.168.0.30
VM_USER = root
VM_PATH = /home/davgalle42/Downloads/ft_shield_update

SRC_SERVER = src/main.c
SRC_CLIENT = src/knock_knock.c src/socket.c

OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

all: $(CLIENT) deploy_and_build_server

# Compilación nativa del cliente en tu Host
$(CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $(CLIENT) $(OBJ_CLIENT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

deploy_and_build_server:
	@echo "📡 Conectando con la VM ($(VM_IP))..."
	@ssh $(VM_USER)@$(VM_IP) "mkdir -p $(VM_PATH)/src $(VM_PATH)/include"
	@scp $(SRC_SERVER) $(VM_USER)@$(VM_IP):$(VM_PATH)/src/
	@scp include/ft_shield.h $(VM_USER)@$(VM_IP):$(VM_PATH)/include/
	@echo "🛠️ Compilando $(SERVER) dentro de la VM de forma remota..."
	@ssh $(VM_USER)@$(VM_IP) "$(CC) $(CFLAGS) -I $(VM_PATH)/include $(VM_PATH)/src/main.c -o $(VM_PATH)/$(SERVER)"
	@echo "✅ ¡Servidor compilado con éxito en la ruta de actualización: $(VM_PATH)/$(SERVER)!"

clean:
	$(RM) $(OBJ_SERVER)
	$(RM) $(OBJ_CLIENT)

fclean: clean
	$(RM) $(CLIENT)
	$(RM) $(SERVER)
	@# Limpieza de los archivos temporales en la VM al hacer fclean
	@ssh $(VM_USER)@$(VM_IP) "rm -rf $(VM_PATH)" 2>/dev/null || true

re: fclean all

.PHONY: all clean fclean re deploy_and_build_server