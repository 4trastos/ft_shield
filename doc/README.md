## Fase 1: El binario "Infección" (Primera ejecución)

Este es el archivo ejecutable que compilamos inicialmente y ejecutamos de forma manual con privilegios de administrador. Su único objetivo en este punto es preparar el terreno dentro de la máquina virtual.

1. 
**Verificación de privilegios:** Lo primero que hace nuestro programa al arrancar es comprobar si se ha ejecutado con permisos de administrador (mediante `getuid()`). Si no somos `root`, el programa debe terminar inmediatamente sin realizar ninguna acción.


2. 
**El "Señuelo" (Stdout):** Imprimimos nuestro login de la escuela por la salida estándar. Esto es lo único que el usuario evaluador debe ver visualmente en su terminal durante esta primera fase.


3. 
**Infiltración del payload:** En segundo plano, nuestro programa abre su propio ejecutable (accediendo a `/proc/self/exe`), lee sus propios bytes y se clona a sí mismo en la ruta de binarios del sistema operativo (por ejemplo, `/bin/ft_shield`).


4. 
**Instalación de la Persistencia:** Escribimos un script o un archivo de configuración de servicio (ya sea en `/etc/init.d/` o un archivo `.service` en `systemd`) para asegurar que ese nuevo binario clonado en la ruta del sistema se ejecute automáticamente cada vez que la máquina se encienda.


5. 
**Salida limpia:** El binario original termina su ejecución sin provocar ningún tipo de alerta o mensaje de error, dejando la máquina lista para el siguiente reinicio.



---

## Fase 2: El binario "Daemon" (Arranque del Sistema)

Cuando la máquina virtual se reinicia, el sistema operativo lee el servicio que configuramos en la Fase 1 y ejecuta de forma automática el binario `/bin/ft_shield`. Aquí el comportamiento del programa cambia por completo al detectar que no ha sido invocado por un usuario en una terminal.

6. 
**Daemonización:** Realizamos el proceso clásico de desvinculación de la terminal para convertirnos en un demonio en segundo plano: ejecutamos un `fork()`, hacemos que el proceso padre muera, el proceso hijo ejecuta `setsid()` para crear una nueva sesión independiente, y redirigimos los descriptores estándar (`stdin`, `stdout`, `stderr`) hacia `/dev/null`.


7. 
**Control de Instancia Única (Singleton):** Intentamos crear y bloquear un archivo específico en el sistema (un archivo PID en `/var/run/ft_shield.pid`). Si no podemos aplicar el bloqueo porque ya existe, significa que ya hay un demonio idéntico corriendo en el sistema y nuestro proceso actual debe morir silenciosamente para cumplir la regla de una sola instancia activa.



---

## Fase 3: El Servidor y la Shell Remota

Una vez establecidos de forma estable en el fondo del sistema operativo, activamos la infraestructura de red del demonio.

8. 
**Creación del Socket Escucha:** Levantamos un servidor TCP, enlazamos (*bind*) el puerto específico `4242` y ponemos el socket en modo de escucha (*listen*).


9. 
**Gestión de Clientes Múltiples:** Entramos en un bucle infinito utilizando un multiplexor de conexiones (como las funciones `select()` o `poll()`) para ser capaces de recibir, aceptar (`accept()`) y gestionar hasta 3 conexiones de clientes de forma simultánea sin bloquear el flujo principal.


10. 
**Control de Acceso (Contraseña Segura):** Cuando un cliente se conecta a nosotros (por ejemplo, utilizando `nc`) , le solicitamos una clave. Aplicamos una función de hash (como SHA-256) a los caracteres que introduce el usuario y los comparamos con el hash seguro que hemos dejado guardado estrictamente dentro del ejecutable, garantizando que la contraseña jamás viaje o se almacene en texto claro.


11. 
**Duplicación de Descriptores (`dup2`):** Si la contraseña es correcta y el cliente nos solicita explícitamente el despliegue de la shell (mediante comandos de control de texto que hayamos definido) , utilizamos la llamada al sistema `dup2()` para redirigir la entrada, salida y error estándar (0, 1 y 2) de ese hilo o proceso directamente hacia el descriptor del socket del cliente que está conectado.


12. 
**Invocación de la Shell de Root:** Finalmente, ejecutamos un `execve("/bin/sh", NULL, NULL)`. Al haber redirigido los descriptores en el paso anterior, el cliente remoto que está escuchando al otro lado de la red recibirá los datos del sistema y obtendrá una terminal interactiva con plenos derechos de `root`.



```bash
Atacante ejecuta ft_shield → máquina infectada
                              → daemon arranca en cada boot
                              → atacante se conecta por red al puerto 4242
                              → introduce password
                              → tiene shell root
```
```bash
nc ip_victima 4242
Keycode: mipassword
$> shell
# ahora tenemos una shell root en la máquina víctima
```