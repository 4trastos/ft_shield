#include "../include/ft_shield.h"

int main()
{
    t_troyan *shield;

    if (getuid())
    {
        printf("Estos son los permisos de usuario ( %d )\n", getuid());
        return (1);
    }
    shield = malloc(sizeof(t_troyan));
    if (!shield)
        return (1);
    shield->id = getuid();
    printf("Estos son los permisos de usuario ( %d )\n", shield->id);
    free(shield);
    return (0);
}

/*

## Paso 1: 
Crearemos la estructura básica del programa en C para que verifique los permisos de `root` 
y simplemente imprima nuestro login por pantalla.

## Paso 2: 
Añadiremos la lógica de clonación (lectura de `/proc/self/exe` 
y escritura en la ruta del sistema).
 
## Paso 3:
Desarrollaremos el mecanismo de persistencia para el arranque
(creación del script en `init.d` o el servicio en `systemd`).

## Paso 4:
Implementaremos la daemonización del proceso y el archivo de bloqueo (*PID lock file*).

## Paso 5:
Levantaremos el servidor de red con el gestor multicliente e integraremos 
la seguridad de la contraseña y el despliegue de la shell de root.

*/