SERVER = ft_shield
CLIENT = ft_backdoor
CC = gcc 
CFLAGS = -Wall -Wextra -Werror -g3
RM = rm -f

VM_IP = 192.168.0.36
VM_USER = root
VM_PATH = /home/davgalle42/Downloads/ft_shield_update

SRC_SERVER = src/main.c src/socket_server.c src/signal.c src/daemon.c src/auth.c src/sha256.s src/listener.c
SRC_CLIENT = src/knock_knock.c src/socket.c src/shell_control.c
SRC_ASM_CLIENT = src/sha256.s

OBJ_SERVER = 
OBJ_CLIENT = $(SRC_CLIENT:.c=.o) src/sha256.o

all: $(CLIENT) deploy_and_build_server

# Compilación nativa del cliente en tu Host
$(CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $(CLIENT) $(OBJ_CLIENT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

src/sha256.o: src/sha256.s
	nasm -f elf64 $< -o $@

deploy_and_build_server:
	@echo "📡 Conectando con la VM ($(VM_IP))..."
	@ssh $(VM_USER)@$(VM_IP) "mkdir -p $(VM_PATH)/src $(VM_PATH)/include"
	@scp $(SRC_SERVER) $(VM_USER)@$(VM_IP):$(VM_PATH)/src/
	@scp include/ft_shield.h $(VM_USER)@$(VM_IP):$(VM_PATH)/include/
	@echo "🛠️ Compilando $(SERVER) dentro de la VM de forma remota..."
	@ssh $(VM_USER)@$(VM_IP) "nasm -f elf64 $(VM_PATH)/src/sha256.s -o $(VM_PATH)/src/sha256.o && \
		$(CC) $(CFLAGS) -I $(VM_PATH)/include $(VM_PATH)/src/main.c $(VM_PATH)/src/socket_server.c $(VM_PATH)/src/signal.c $(VM_PATH)/src/daemon.c $(VM_PATH)/src/listener.c $(VM_PATH)/src/auth.c $(VM_PATH)/src/sha256.o -o $(VM_PATH)/$(SERVER)"
	@echo "✅ ¡Servidor compilado con éxito en la ruta de actualización: $(VM_PATH)/$(SERVER)!"

clean:
	$(RM) $(OBJ_CLIENT)

fclean: clean
	$(RM) $(CLIENT)
	$(RM) $(SERVER)
	@# Limpieza de los archivos temporales en la VM al hacer fclean
	@ssh $(VM_USER)@$(VM_IP) "rm -rf $(VM_PATH)" 2>/dev/null || true

re: fclean all

.PHONY: all clean fclean re deploy_and_build_server