/*#######################################################################################
 #  Fecha: 19 de Mayo 2026
 #
 #* Autor: Nicholas Ruiz, Federico Restrepo
 #* 	 
 #               Pontificia Universidad Javeriana
 #*	 	
 #* Descripción:
 #* Genera 10 números secuenciales y los guarda en un búfer circular en memoria compartida.
 #* Usa los semáforos '/vacio', '/lleno' y '/mutex_sem' para no sobreescribir datos y coordinarse de forma segura con el consumidor.
 # 
######################################################################################*/


#include "comun.h"

int main() {
    // Crea los semáforos con nombre de POSIX
    sem_t *vacio = sem_open("/vacio", O_CREAT, 0644, BUFFER);
    sem_t *lleno = sem_open("/lleno", O_CREAT, 0644, 0);
    sem_t *mutex = sem_open("/mutex_sem", O_CREAT, 0644, 1);

    // Validación de la correcta apertura de los semáforos
    if (vacio == SEM_FAILED || lleno == SEM_FAILED || mutex == SEM_FAILED) {
        perror("Error al abrir sem_open en el Productor");
        exit(EXIT_FAILURE);
    }

    // Crear  el objeto de memoria 
    int shm_fd = shm_open("/memoria_compartida", O_CREAT | O_RDWR, 0644);
    if (shm_fd < 0) {
        perror("Error en shm_open del Productor");
        exit(EXIT_FAILURE);
    }
    
    // Definir el tamaño de la memoria compartida
    ftruncate(shm_fd, sizeof(compartir_datos));
 
    // Mapear el objeto de memoria compartida en el espacio de direcciones del proceso
    compartir_datos *compartir = mmap(NULL, sizeof(compartir_datos), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (compartir == MAP_FAILED) {
        perror("Error en mmap del Productor");
        exit(EXIT_FAILURE);
    }

    // Inicializar el índice de entrada
    compartir->entrada = 0;

    // Ciclo de producción (Produce 10 elementos numerados del 1 al 10)
    for (int i = 1; i <= 10; i++) {
        sem_wait(vacio);  // Decrementa los espacios vacíos
        sem_wait(mutex);  // Entra a la Sección Crítica 

        
        compartir->bus[compartir->entrada] = i; 
        printf("Productor: Produce %d en posición %d\n", i, compartir->entrada);
        
        // Avanzar el índice de forma circular 
        compartir->entrada = (compartir->entrada + 1) % BUFFER;
       

        sem_post(mutex);  // Sale de la Sección Crítica
        sem_post(lleno);  // Incrementa los espacios ocupados, notificando al consumidor
        
        sleep(1);  // El tiempo que tarda en producir un nuevo elemento
    }

    // Limpieza de recursos del sistema
    munmap(compartir, sizeof(compartir_datos)); // Desmapear la memoria
    close(shm_fd);                              // Cerrar el descriptor de la memoria
    sem_close(vacio);                           // Cerrar los semáforos en este proceso
    sem_close(lleno);
    sem_close(mutex);

    
    return 0;
}