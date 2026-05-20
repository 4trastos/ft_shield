#include "./include/ft_shield.h"

int main()
{
    return (0);
}


// OBJETIVO CONTROLAR UN PC AJENO:

// **Lo que hace el binario cuando lo ejecutamos:**

// 1. Imprime nuestro login en stdout
// 2. En background hace todo:
//    - Se copia a `/bin/ft_shield`
//    - Se registra para arrancar con el sistema (TENEMOS QUE DECIDIR COMO —> `init.d`, `systemd`, `cron @reboot`...)
//    - Se lanza como daemon

// **Lo que hace el daemon:**

// - Solo puede haber una instancia corriendo (lock file o similar)
// - Escucha en el puerto 4242
// - Acepta máximo 3 conexiones simultáneas
// - Cuando se conecta un cliente pide una contraseña — **cifrada en el código**, no en claro
// - Si la contraseña es correcta ofrece una shell con root — usando `execve` o similar, **no el comando `shell`**
// - Sin mensajes de error en la máquina objetivo


// **Lo que necesitamos saber:**

// - `fork()` + `setsid()` para daemonizar
// - `socket()`, `bind()`, `listen()`, `accept()` para el servidor TCP
// - `execve("/bin/sh", ...)` para la shell
// - Cifrado de contraseña — `crypt()` con sal, o SHA256
// - Persistencia en arranque — `systemd` o `init.d`
// - `getuid()` para verificar que se ejecuta como root


/* Atacante ejecuta ft_shield → máquina infectada
                              → daemon arranca en cada boot
                              → atacante se conecta por red al puerto 4242
                              → introduce password
                              → tiene shell root
*/
/*
nc ip_victima 4242
Keycode: mipassword
$> shell
# ahora tenemos una shell root en la máquina víctima
*/