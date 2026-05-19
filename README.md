# Taller 03 — Sincronización POSIX

**Sistemas Operativos · Pontificia Universidad Javeriana**  
Autores: Nicholas Ruiz Zhilkin · Federico Restrepo Guzmán 
Profesor: John Corredor

---

## Descripción

Este proyecto implementa dos mecanismos clásicos de sincronización en C usando el estándar POSIX:

- **Actividad 1** — Problema Productor/Consumidor con **procesos independientes**, memoria compartida y semáforos con nombre (*Named Semaphores*).
- **Actividad 2** — Problema Multi-Productor/Único Consumidor con **hilos** (`pthreads`), mutex y variables de condición.

---

## Estructura del proyecto

```
Taller_Posix/
├── Makefile
├── README.md
├── bin/                        # Ejecutables generados
│   ├── producer
│   ├── consumer
│   └── posixSincro
├── Actividad_1/                # Procesos + Named Semaphores
│   ├── comun.h                 # Encabezado compartido (buffer, structs)
│   ├── producer.c              # Produce 10 enteros en el búfer circular
│   └── consumer.c              # Consume los 10 enteros del búfer
└── Actividad_2/                # Hilos + Mutex + Variables de condición
    └── posixSincro.c           # 10 hilos productores + 1 hilo spooler
```

---

## Requisitos

- GCC con soporte POSIX (`-lpthread -lrt`)
- Linux (Ubuntu 20.04+ recomendado)
- Make

---

## Compilación

```bash
make          # Compila todo y genera los binarios en bin/
make clean    # Elimina los binarios
```

---

## Ejecución

### Actividad 1 — Dos terminales simultáneas

```bash
# Terminal 1
./bin/producer

# Terminal 2 (al mismo tiempo)
./bin/consumer
```

**Salida esperada:**
```
Productor: Produce 1 en posición 0
Productor: Produce 2 en posición 1
Consumidor: Consume 1 desde posición 0
Productor: Produce 3 en posición 2
...
```

> El productor tarda 1 s/elemento y el consumidor 2 s/elemento.  
> El búfer (tamaño 5) actúa como amortiguador — el productor se bloquea cuando está lleno.

### Actividad 2 — Una sola terminal

```bash
./bin/posixSincro
```

**Salida esperada:**
```
Thread 0: 1
Thread 3: 1
Thread 1: 1
...
```

> Los 10 hilos escriben en desorden (concurrencia real), pero el spooler imprime de forma ordenada y sin pérdida de datos.

---

## Limpieza de recursos POSIX

Si una ejecución termina de forma abrupta, los semáforos y la memoria compartida pueden quedar activos. Elimínalos antes de volver a ejecutar:

```bash
rm -f /dev/shm/memoria_compartida
rm -f /dev/shm/sem.vacio /dev/shm/sem.lleno /dev/shm/sem.mutex_sem
```

---

## Mecanismos de sincronización

### Actividad 1

| Semáforo | Valor inicial | Propósito |
|---|---|---|
| `/vacio` | 5 (= BUFFER) | Espacios libres en el búfer — bloquea al productor si lleno |
| `/lleno` | 0 | Espacios ocupados — bloquea al consumidor si vacío |
| `/mutex_sem` | 1 | Exclusión mutua sobre la sección crítica |

Orden correcto de operaciones en el **productor**:
```
sem_wait(vacio) → sem_wait(mutex) → escribir → sem_post(mutex) → sem_post(lleno)
```

Orden correcto en el **consumidor**:
```
sem_wait(lleno) → sem_wait(mutex) → leer → sem_post(mutex) → sem_post(vacio)
```

> ⚠️ Invertir el orden de `sem_wait` provoca **deadlock**.

### Actividad 2

| Primitivo | Tipo | Propósito |
|---|---|---|
| `buf_mutex` | `pthread_mutex_t` | Exclusión mutua sobre la matriz compartida |
| `buf_cond` | `pthread_cond_t` | Bloquea productores cuando el búfer está lleno |
| `spool_cond` | `pthread_cond_t` | Bloquea al spooler cuando no hay líneas para imprimir |

> Los `while` antes de `pthread_cond_wait` protegen contra **spurious wakeups** (despertares espurios que permite el estándar POSIX).

---

## Parámetros configurables

En `comun.h` (Actividad 1) y `posixSincro.c` (Actividad 2):

```c
#define BUFFER 5       // Tamaño del búfer circular
#define MAX_BUFFERS 5  // Igual en posixSincro.c
```

En el código puedes ajustar los `sleep()` para cambiar la velocidad de producción y consumo y observar distintos comportamientos de sincronización.

---

## Autor

| Nombre | Rol |
|---|---|
| Federico Restrepo Guzmán | Desarrollo y documentación |
| Nicholas Ruiz Zhilkin | Desarrollo y documentación |
| John Corredor (PhD) | Profesor — Sistemas Operativos |

Pontificia Universidad Javeriana · Bogotá · 2026
