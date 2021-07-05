/**
 *
 * Descripcion: Implementacion de la funciones necesarias
 * para el uso de memoria mediante una cola compartida
 *
 * Fichero: shared_queue.c
 * Autor: Claudia González Arteaga y Laura Iniesta Rodríguez
 * Version: 1.0
 * Fecha: 04-2020
 *
 */
#include "shared_queue.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>



#define QUEUE_SHM_NAME "/queue_shm"
#define QUEUE_SEM_MUTEX_NAME "/queue_sem_mutex"
#define QUEUE_SEM_FILL_NAME "/queue_sem_fill"
#define QUEUE_SEM_EMPTY_NAME "/queue_sem_empty"



/****************************************************/
/* Funcion: SharedQueue_init  Fecha:04/2020         */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que inicializa los semáforos de la cola   */
/* compartida a sem_failed                          */
/*                                                  */
/* Entrada:                                         */
/* SharedQueue *queue: puntero a la cola            */
/****************************************************/
static void SharedQueue_init(SharedQueue *queue){
  if(!queue){
    printf("Error abriendo la cola para inicializarla. ");
  }
    queue->mutex=SEM_FAILED;
    queue->fill_count=SEM_FAILED;
    queue->empty_count=SEM_FAILED;
    queue->memory=NULL;

}

/****************************************************/
/* Funcion: SharedQueue_create  Fecha:04/2020       */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que crea la cola, reserva memoria y abre  */
/* los semáforos y la memoria compartida            */
/*                                                  */
/* Entrada:                                         */
/* SharedQueue **queue: puntero a la cola           */
/* pid_t id: Id del proceso que la crea             */
/****************************************************/

void SharedQueue_create(SharedQueue **queue,pid_t id){
    int fd_shm;

    *queue = (SharedQueue*)malloc(sizeof(SharedQueue));
    if(!queue){
      fprintf(stdout, "Error reservando memoria para queue. \n");
    }
    SharedQueue_init(*queue);

    fd_shm = shm_open(QUEUE_SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    if (fd_shm == -1) {
      if (errno == EEXIST) {
        fd_shm = shm_open(QUEUE_SHM_NAME, O_RDWR, 0);
        if (fd_shm == -1) {
            fprintf(stderr, "Error abriendo el segmento de memoria compartida\n");
            SharedQueue_delete(*queue);
            exit(EXIT_FAILURE);
        }
      }
      else {
          fprintf(stderr, "Error redimensionando el segmento de memoria compartida\n");
          SharedQueue_delete(*queue);
          exit(EXIT_FAILURE);
      }

    }

    if (ftruncate(fd_shm, sizeof((*queue)->memory)) == -1) { //no se si esta bien meter queue en sizeof
        fprintf(stderr, "Error al redimensionar los segmen de memoria compartida\n");
        SharedQueue_delete(*queue);
        shm_unlink(QUEUE_SHM_NAME);
        exit(EXIT_FAILURE);
    }

      (*queue)->memory = mmap(NULL, sizeof((*queue)->memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);//no se si esta bien meter queue->memory en sizeof
      close(fd_shm);

      if ((*queue)->memory == MAP_FAILED) {
          fprintf(stderr, "Error al mapear el segmento de memoria compartida\n");
          SharedQueue_delete(*queue);
        	exit(EXIT_FAILURE);
      }
    (*queue)->memory->id= id;
    (*queue)->memory->head = (*queue)->memory->count = 0;

    sem_unlink(QUEUE_SEM_MUTEX_NAME);
    if (((*queue)->mutex = sem_open(QUEUE_SEM_MUTEX_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
      perror("sem_open_mutex");
      exit(EXIT_FAILURE);
      }
      sem_unlink(QUEUE_SEM_FILL_NAME);

      if (((*queue)->fill_count = sem_open(QUEUE_SEM_FILL_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("sem_open_fillcount");
        exit(EXIT_FAILURE);
      }

      sem_unlink(QUEUE_SEM_EMPTY_NAME);
      if (((*queue)->empty_count = sem_open(QUEUE_SEM_EMPTY_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, QUEUE_BUFFER_MAX)) == SEM_FAILED) {
        perror("sem_open_emptycount");
        exit(EXIT_FAILURE);
      }
}

/****************************************************/
/* Funcion: SharedQueue_open    Fecha:04/2020       */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que abre la cola, reserva memoria y abre  */
/* los semáforos y la memoria compartida            */
/*                                                  */
/* Entrada:                                         */
/* SharedQueue **queue: puntero a la cola           */
/****************************************************/
void SharedQueue_open(SharedQueue **queue){
    int fd_shm;
    *queue = (SharedQueue*)malloc(sizeof(SharedQueue));
    if(!queue) {
      printf("Error reservando memoria para la cola\n");
    }

    SharedQueue_init(*queue);

    fd_shm = shm_open(QUEUE_SHM_NAME, O_RDWR, 0); //MIRAR ARRGUMENTOS
    if (fd_shm == -1) {
        fprintf(stderr, "Error al crear el segmento de memoria compartida\n");
    }

    (*queue)->memory = mmap(NULL, sizeof((*queue)->memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);//no se si esta bien meter queue->memory en sizeof
    if ((*queue)->memory == MAP_FAILED) {
        fprintf(stderr, "Error al mapear el segmento de memoria compartida\n");
        shm_unlink(QUEUE_SHM_NAME);
    }
    (*queue)->memory->head=0;

    if (((*queue)->mutex = sem_open(QUEUE_SEM_MUTEX_NAME, 1)) == SEM_FAILED) { //MIRAR LOS PERMISOS
      perror("sem_open_mutex");
      exit(EXIT_FAILURE);
    }

    if (((*queue)->fill_count = sem_open(QUEUE_SEM_FILL_NAME, QUEUE_BUFFER_MAX)) == SEM_FAILED) {
      perror("sem_open_fillcount");
      exit(EXIT_FAILURE);
    }


    if (((*queue)->empty_count = sem_open(QUEUE_SEM_EMPTY_NAME, QUEUE_BUFFER_MAX)) == SEM_FAILED) {
      perror("sem_open_emptycount");
      exit(EXIT_FAILURE);
    }

}

/****************************************************/
/* Funcion: SharedQueue_delete  Fecha:04/2020       */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que libera los recursos utilizados        */
/*                                                  */
/* Entrada:                                         */
/* SharedQueue *queue: puntero a la cola           */
/****************************************************/

void SharedQueue_delete(SharedQueue *queue){

    if(queue->mutex != SEM_FAILED){
      sem_close(queue->mutex);
      sem_unlink(QUEUE_SEM_MUTEX_NAME);
    }
    if(queue->fill_count != SEM_FAILED){
      sem_close(queue->fill_count);
      sem_unlink(QUEUE_SEM_FILL_NAME);
    }
    if(queue->empty_count != SEM_FAILED){
      sem_close(queue->empty_count);
      sem_unlink(QUEUE_SEM_EMPTY_NAME);
    }
    if(queue->memory != NULL){
      munmap(queue->memory, sizeof(queue->memory));
      shm_unlink(QUEUE_SHM_NAME);
    }
    free(queue);

}

/****************************************************/
/* Funcion: SharedQueue_close   Fecha:04/2020       */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que cierra los recursos utilizados        */
/*                                                  */
/* Entrada:                                         */
/* SharedQueue *queue: puntero a la cola            */
/****************************************************/

void SharedQueue_close(SharedQueue *queue){
  if(queue->mutex != SEM_FAILED){
    sem_close(queue->mutex);
  }
  if(queue->fill_count != SEM_FAILED){
    sem_close(queue->fill_count);
  }
  if(queue->empty_count != SEM_FAILED){
    sem_close(queue->empty_count);
  }
  if(queue->memory != NULL){
    munmap(queue->memory, sizeof(queue->memory));
  }

}


/****************************************************/
/* Funcion: SharedQueue_enqueue  Fecha:04/2020      */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que escribe en el buffer de la memoria    */
/* de la cola                                       */
/*                                                  */
/* Entrada:                                         */
/* SharedQueue *queue: puntero a la cola            */
/* int element: elemento a escribir en el buffer    */
/****************************************************/

void SharedQueue_enqueue(SharedQueue *queue, int element) {
  int pos=0;

  if(sem_wait(queue->empty_count) < 0){
    perror("sem_wait1 " );
    sem_close(queue->empty_count);
    exit(EXIT_FAILURE);
  }
  if(sem_wait(queue->mutex) < 0){
    perror("sem_wait1 " );
    sem_close(queue->mutex);
    exit(EXIT_FAILURE);
  }
//Obtenemos la posicion en la que insertaremos el elemento
pos = queue->memory->count;

//escribimos en la posición el elemento
queue->memory->buffer[pos] = element;

//aumentamos el count de la cola
queue->memory->count++;
queue->memory->count %= QUEUE_BUFFER_MAX;

  if(sem_post(queue->mutex) < 0){
    perror("sem_wait post " );
    sem_close(queue->mutex);
  }
  if(sem_post(queue->fill_count) < 0){
    perror("sem_wait post " );
    sem_close(queue->fill_count);
  }
}

/****************************************************/
/* Funcion: SharedQueue_dequeue  Fecha:04/2020      */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que lee del buffer de la memoria          */
/* de la cola                                       */
/*                                                  */
/* Entrada:                                         */
/* SharedQueue *queue: puntero a la cola            */
/* Salida:                                          */
/* int ret: elemento que ha leido del buffer        */
/****************************************************/
int SharedQueue_dequeue(SharedQueue *queue){
  int ret;

  if(sem_wait(queue->fill_count) < 0){
    perror("sem_wait1 " );
    sem_close(queue->fill_count);
    exit(EXIT_FAILURE);
  }

  if(sem_wait(queue->mutex) < 0){
    perror("sem_wait1 " );
    sem_close(queue->mutex);
    exit(EXIT_FAILURE);
  }


  ret = queue->memory->buffer[queue->memory->head];
  queue->memory->head++;
  queue->memory->head %= QUEUE_BUFFER_MAX;


  if(sem_post(queue->mutex) < 0){
    perror("sem_wait post " );
    sem_close(queue->mutex);
  }
;
  if(sem_post(queue->empty_count) < 0){
    perror("sem_wait post " );
    sem_close(queue->empty_count);
  }
  printf("Nuevo espacio en la cola. \n");

  return ret;
}
