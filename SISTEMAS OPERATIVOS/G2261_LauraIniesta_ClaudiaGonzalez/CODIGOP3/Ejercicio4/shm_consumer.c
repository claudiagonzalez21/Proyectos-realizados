/**
 *
 * Descripcion: Implementacion de un programa que vacía una
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

/****************************************************/
/* Funcion: main       Fecha:04/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que extrae números de una SharedQueue     */
/* y envía una señal al creador de la cola cuando   */
/* acaba                                            */
/* Salida:                                          */
/* int: 0 con éxito -1 con error                    */
/****************************************************/
int main(void){
  int i, res, valores[10] = {0};
  SharedQueue *queue=NULL;

  for(i=0; i<10; i++){
    valores[i]=0;
  }
  SharedQueue_open(&queue);

  do {
    res = SharedQueue_dequeue(queue);
    printf("numero extraido: %d\n", res);
    if(res > -1) {
      valores[res]++;
    }
  } while(res != -1);

  for(i=0; i<=10; i++){
    printf("%d, ", queue->memory->buffer[i]);
  }
  printf("\n");
  for (i = 0; i < 10; i++) {
      printf("Número: %d, número de veces que ha aparecido: %d\n", i, valores[i]);
  }

  SharedQueue_close(queue);
  return 0;
}
