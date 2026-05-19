/*********************************************************************************************
 #* Fecha: 19 de Mayo 2026
 #
 #* Autor: Nicholas Ruiz, Federico Restrepo
 #* 	 
 #               Pontificia Universidad Javeriana
 #
 * Tema:     Encabezado compartido - Problema Productor/Consumidor con semáforos POSIX
 *
 * Descripción:
 *   Define las interfaces, constantes y estructuras de datos compartidas entre
 *   el proceso productor y el proceso consumidor. Ambos procesos deben incluir
 *   este archivo para garantizar consistencia en el acceso a la memoria compartida.
 *********************************************************************************************/

#ifndef COMUN_H
#define COMUN_H

#include <semaphore.h>   /* sem_t, sem_open, sem_wait, sem_post, sem_close, sem_unlink */
#include <fcntl.h>       /* O_CREAT, O_RDWR                                            */
#include <sys/mman.h>    /* shm_open, mmap, munmap, shm_unlink                         */
#include <stdio.h>       /* printf, perror                                              */
#include <stdlib.h>      /* exit, EXIT_FAILURE                                          */
#include <unistd.h>      /* sleep, close, ftruncate                                     */

/** Tamaño máximo del búfer circular compartido. */
#define BUFFER 5


typedef struct {
    int bus[BUFFER];
    int entrada;
    int salida;
} compartir_datos;

#endif /* COMUN_H */
