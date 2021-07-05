/**
 *
 * Descripcion: Implementacion de un programa que llena una
 * cola de memoria compartida
 *
 * Fichero: shm_consumer.c
 * Autor: Claudia González Arteaga y Laura Iniesta Rodríguez
 * Version: 1.0
 * Fecha: 04-2020
 *
 */
#include "shared_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


/****************************************************/
/* Funcion: manejador  Fecha:04/2020        */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que se ejecuta si se recibe la señal      */
/* SIGUSR 1                                         */
/* Entrada:                                         */
/* int sig: señal que se ha recibido                */
/****************************************************/

void manejador (int sig) {
	if (sig == SIGUSR1){
		printf("\nRecibida la señal SIGUSR1, el consumidor termina\n");
	}

}

/****************************************************/
/* Funcion: main       Fecha:04/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que mete números en una SharedQueue       */
/* y espera una señal del consumer de esa cola      */
/*                                                  */
/* Entrada:                                         */
/* int argc: Número de argumentos                   */
/* char* argv[]: argumentos                         */
/* Salida:                                          */
/* int: 0 con éxito -1 con error                    */
/****************************************************/
int main(int argc, char *argv[]){
  int i, N, A, *valores;
  SharedQueue *queue=NULL;

  if(argc != 3){
  			printf("Error. Número incorrecto de argumentos. \n");
  			return -1;
  		}

  N=atoi(argv[1]);
  if(N<=0){
    printf("Error. Debes escribir un numero mayor que 0. \n");
  }
  A=atoi(argv[2]);

	if( N < 0 || A < 0 || A > 1) {
    fprintf(stdout, "ERROR: Debe ser %s <N> <A> (con N > 0 y el valor de A 0 o 1).\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  valores = (int*)malloc(N*sizeof(int));
  if(!valores){
    return -2;
  }

  SharedQueue_create(&queue, getpid());

  for(i=0; i<N; i++){
    if(A == 0){
        valores[i] = rand()%10;
				printf("Valores generados %d\n", valores[i]);
    }
    else if(A==1){
        valores[i]=i;
				printf("Valores generados %d\n", valores[i]);
    }
  }
  for(i=0; i<N; i++){
    SharedQueue_enqueue(queue, valores[i]); //  Ahora de 0 a 9 llamo a encolar (SharedQueue_enqueue)  el número correspondiente

  }



  SharedQueue_enqueue(queue, -1);
	printf("Numeros: ");
  for(i=0; i<=N; i++){
    printf("%d, ", queue->memory->buffer[i]);
  }


  SharedQueue_delete(queue);
	printf("Finalizado.\n");

  return 0;

}
