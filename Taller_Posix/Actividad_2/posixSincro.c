/*#######################################################################################
 #* Fecha: 19 de Mayo 2026
 #
 #* Autor: Nicholas Ruiz, Federico Restrepo
 #* 	 
 #               Pontificia Universidad Javeriana
 #
 #* Descripción:
 #         Lanza 10 hilos que escriben textos y un hilo impresor que los muestra en pantalla usando una matriz compartida.
 #         Usa un Mutex (candado) y dos variables de condición para darse los turnos de forma ordenada y sin errores.
######################################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFERS 5 // Define el tamaño máximo del búfer 


char buf [MAX_BUFFERS] [100];
int buffer_index;       // escritura para los hilos productores
int buffer_print_index; // lectura para el hilo 


 
 

 
pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER; // Asegura exclusión mutua para que solo un hilo modifique las variables globales a la vez.
pthread_cond_t buf_cond = PTHREAD_COND_INITIALIZER; // Condición para bloquear a los productores si el búfer está lleno.
pthread_cond_t spool_cond = PTHREAD_COND_INITIALIZER; // Condición para bloquear al spooler si no hay líneas para imprimir.

int buffers_available = MAX_BUFFERS; // Contador de espacios libres en el búfer
int lines_to_print = 0;              // Contador de líneas depositadas 

void *producer (void *arg);
void *spooler (void *arg);

int main (int argc, char **argv){
    pthread_t tid_producer [10], tid_spooler; // Arreglos para almacenar los identificadores de los hilos
    int i, r;

    buffer_index = buffer_print_index = 0; // Inicialización de los punteros del búfer circular

    // Creación del hilo Consumidor
    if ((r = pthread_create (&tid_spooler, NULL, spooler, NULL)) != 0) {
        fprintf (stderr, "Error = %d (%s)\n", r, strerror (r)); exit (1);
    }

    int thread_no [10];
    // Creación de 10 hilos Productores concurrentes
    for (i = 0; i < 10; i++) {
        thread_no [i] = i; // Asignación de ID único para cada hilo
        if ((r = pthread_create (&tid_producer [i], NULL, producer, (void *) &thread_no [i])) != 0) {
            fprintf (stderr, "Error = %d (%s)\n", r, strerror (r)); exit (1);
        }
    }

    // Espera  para que los 10 hilos productores terminen su ejecución
    for (i = 0; i < 10; i++)
        if ((r = pthread_join (tid_producer [i], NULL)) == -1) {
            fprintf (stderr, "Error = %d (%s)\n", r, strerror (r)); exit (1);
        }
    
    // Asegura que el spooler termine de imprimir todo antes de finalizar
    while (lines_to_print) sleep (1);

    // Cancelación forzosa del hilo spooler 
    if ((r = pthread_cancel (tid_spooler)) != 0) {
        fprintf (stderr, "Error = %d (%s)\n", r, strerror (r)); exit (1);
    }

    exit (0);
}

void *producer (void *arg){
    int i, r;
    int my_id = *((int *) arg); 
    int count = 0;

    for (i = 0; i < 10; i++) {

        // Solicitar el Mutex 
        if ((r = pthread_mutex_lock (&buf_mutex)) != 0) {
            fprintf (stderr, "Error = %d (%s)\n", r, strerror (r)); exit (1);
        }
            
            
            
           
             
             // Si no hay espacios disponibles, se llama a pthread_cond_wait 
            while (!buffers_available) 
                pthread_cond_wait (&buf_cond, &buf_mutex);

            int j = buffer_index;
            buffer_index++;
            if (buffer_index == MAX_BUFFERS) 
                buffer_index = 0;
            buffers_available--; // Se reduce un espacio libre

            // Escritura en la memoria compartida por hilos
            sprintf (buf [j], "Thread %d: %d\n", my_id, ++count);
            lines_to_print++; // Se incrementa una línea disponible

            // Notificar al hilo spooler bloqueado que ya hay datos para procesar
            pthread_cond_signal (&spool_cond);

    
        if ((r = pthread_mutex_unlock (&buf_mutex)) != 0) {
            fprintf (stderr, "Error = %d (%s)\n", r, strerror (r)); exit (1);
        }
    
        sleep (1); //  El tiempo que tarda en producir un nuevo elemento
    }
    return NULL;
}

void *spooler (void *arg){
    int r;

    while (1) {  
        // Adquisición del candado de exclusión mutua
        if ((r = pthread_mutex_lock (&buf_mutex)) != 0) {
            fprintf (stderr, "Error = %d (%s)\n", r, strerror (r)); exit (1);
        }
            // Si no hay líneas producidas para imprimir, el hilo spooler se suspende eficientemente
            while (!lines_to_print) 
                pthread_cond_wait (&spool_cond, &buf_mutex);

            // Operación de consumo (Lectura e impresión en pantalla)
            printf ("%s", buf [buffer_print_index]);
            lines_to_print--;

            buffer_print_index++;
            if (buffer_print_index == MAX_BUFFERS) // Lógica circular del búfer de lectura
               buffer_print_index = 0;

            buffers_available++; // Se restaura un espacio libre en el búfer
            
            // Notificar a los productores bloqueados que se ha liberado un espacio en la matriz
            pthread_cond_signal (&buf_cond);

        // Liberación del candado
        if ((r = pthread_mutex_unlock (&buf_mutex)) != 0) {
            fprintf (stderr, "Error = %d (%s)\n", r, strerror (r)); exit (1);
        }
    }
    return NULL;
}