# ft_shield

## Descripción

`ft_shield` es un **troyano simple** escrito en C o ASM. Un troyano es un tipo de malware que se disfraza de software legítimo para instalarse en el sistema de la víctima sin que esta lo sepa. El programa visible es inofensivo — solo muestra el login del usuario — pero en segundo plano instala y ejecuta un daemon malicioso.

## Funcionamiento

Cuando se ejecuta `ft_shield` con permisos de root ocurren dos cosas:

1. **Lo que ve el usuario:** el programa muestra el login en la salida estándar y termina.
2. **Lo que ocurre en segundo plano:** el programa copia un binario también llamado `ft_shield` en el directorio de binarios del sistema operativo, lo registra para que se ejecute al arranque y lo lanza como daemon.

## El daemon

El daemon que se instala en el sistema tiene las siguientes características:

- Solo puede haber **una instancia** en ejecución simultáneamente.
- Escucha en el **puerto 4242**.
- Acepta hasta **3 conexiones simultáneas** de clientes.
- Al conectarse un cliente se solicita una **contraseña** — almacenada de forma segura, nunca en claro en el código.
- Una vez autenticado, el cliente puede lanzar una **shell con permisos de root**.

## Requisitos técnicos

- Lenguaje: **C** (cualquier versión) o **ASM**
- Se entrega un **Makefile** respetando las normas habituales de 42
- La elección del sistema operativo es libre
- La evaluación se realizará en una VM
- **No deben aparecer mensajes de error** durante la creación ni el uso del binario en la máquina objetivo
- No se pueden usar librerías que realicen el trabajo sucio — se considera trampa

## Estructura del repositorio

```
ft_shield/
├── Makefile
├── README.md
├── include/
│   └── ft_shield.h
└── src/
    └── *.c
```

## Uso

```bash
# Compilar
make

# Ejecutar (requiere permisos de root)
sudo ./ft_shield

# Verificar que el daemon está corriendo
service --status-all | grep ft_shield

# Conectarse al daemon
nc localhost 4242
```

## Bonus

- Cuantificar los datos de entrada/salida
- Añadir funciones adicionales al daemon (logging de acciones del usuario)
- Disfrazar el troyano de forma que sea difícil de detectar
- Aplicar técnicas de packing para reducir el tamaño del binario
- Optimizar el ejecutable para dificultar su detección

> **Nota:** Los bonus solo se evalúan si la parte obligatoria está completamente implementada y funciona sin errores.
