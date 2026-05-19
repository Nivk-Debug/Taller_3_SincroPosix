/*#######################################################################################
 #* Fecha: 19 de Mayo 2026
 #
 #* Autor: Nicholas Ruiz, Federico Restrepo
 #* 	 
 #               Pontificia Universidad Javeriana
 #* Descripción:
 #*      Extrae 10 números del búfer circular en memoria compartida para mostrarlos en pantalla.
 #*      Usa los semáforos '/vacio', '/lleno' y '/mutex_sem' para evitar leer datos inexistentes y sincronizarse con el productor.
######################################################################################*/



#include "comun.h"

int main() {
    // Abrir los semáforos 
    sem_t *vacio = sem_open("/vacio", 0);
    sem_t *lleno = sem_open("/lleno", 0);
    sem_t *mutex = sem_open("/mutex_sem", 0);

    if (vacio == SEM_FAILED || lleno == SEM_FAILED || mutex == SEM_FAILED) {
        perror("Error al abrir sem_open en el Consumidor");
        exit(EXIT_FAILURE);
    }

    //  Abrir el objeto de memoria 
    int fd_compartido = shm_open("/memoria_compartida", O_RDWR, 0644);
    if (fd_compartido < 0) {
        perror("Error en shm_open del Consumidor");
        exit(EXIT_FAILURE);
    }

    // Mapear la memoria 
    compartir_datos *compartir = mmap(NULL, sizeof(compartir_datos), PROT_READ | PROT_WRITE, MAP_SHARED, fd_compartido, 0);
    if (compartir == MAP_FAILED) {
        perror("Error en mmap del Consumidor");
        exit(EXIT_FAILURE);
    }

    // Inicializar el índice de salida
    compartir->salida = 0;


    for (int i = 1; i <= 10; i++) {
        sem_wait(lleno);  // Decrementa los espacios ocupados. 
        sem_wait(mutex);  // Entra a la Sección Crítica 

        
        int item = compartir->bus[compartir->salida]; // Recupera el dato del búfer
        printf("Consumidor: Consume %d desde posición %d\n", item, compartir->salida);
        
        // Avanzar el índice de forma circular
        compartir->salida = (compartir->salida + 1) % BUFFER;
    

        sem_post(mutex);  // Sale de la Sección Crítica
        sem_post(vacio);  // Incrementa los espacios vacíos
        
        sleep(2);  //  tiempo que tarda el consumidor en procesar el dato
    }

    // Eliminar los recursos del sistema 
    munmap(compartir, sizeof(compartir_datos));
    close(fd_compartido);
    
    sem_close(lleno);
    sem_close(vacio);
    sem_close(mutex);
    
    // Eliminar los nombres del sistema 
    sem_unlink("/vacio");
    sem_unlink("/lleno");
    sem_unlink("/mutex_sem");
    shm_unlink("/memoria_compartida");

    return 0;
}