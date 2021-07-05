/**
 *
 * Descripcion: Implementacion de un programa que envía mensajes
 * a través de una cola de mensajes
 *
 * Fichero: mq_injector.c
 * Autor: Claudia González Arteaga y Laura Iniesta Rodríguez
 * Version: 1.0
 * Fecha: 04-2020
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define MAX 2048
#define MAX_LECTURA 2000

/**
 * @brief Mensaje ADT.
 */
typedef struct {
    int valor;
    char aviso[MAX];
} Mensaje;


/****************************************************/
/* Funcion: main       Fecha:04/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que lee de fichero y envía como mensaje   */
/* por la cola compartida lo que ha leído           */
/*                                                  */
/* Entrada:                                         */
/* int argc: Número de argumentos                   */
/* char* argv[]: argumentos                         */
/* nombre de fichero y nombre de la cola de mensajes*/
/* Salida:                                          */
/* int: 0 con éxito -1 con error                    */
/****************************************************/

int main(int argc, char * argv[]){
  Mensaje *msg;
  char* fichero;
  char* nombremsg;
  char linea[MAX_LECTURA];
   int fd,i;
   int leido=1, enviados;

  struct mq_attr attributes = {
      .mq_flags = 0,
      .mq_maxmsg = 10,
      .mq_curmsgs = 0,
      .mq_msgsize = sizeof(Mensaje)
  };

  mqd_t queue;

  msg = (Mensaje*)malloc(sizeof(Mensaje));
  if(!msg){
    printf("Error reservando memoria para el mensaje\n");
    return -1;
  }

    if(argc != 3){
      printf("Faltan argumentos de entrada\n");
      printf("Se debe introducir el programa, el nombre del fichero y el de la cola de mensajes \n");
      exit(EXIT_FAILURE);
    }
    fichero = argv[1];
    nombremsg = argv[2];

    if(fichero == NULL || nombremsg == NULL){
      printf("A introducido argumentos no válidos");
      exit(EXIT_FAILURE);
    }

    fd = open(fichero,O_RDONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
      if(fd < 0){
        perror("Open ");
        free(msg);
        return -1;
      }

   queue = mq_open(nombremsg,O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR,&attributes);

    if (queue == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        free(msg);
        close(fd);
        mq_unlink(nombremsg);
        return EXIT_FAILURE;
    }

    enviados=0;

    while(leido != 0){
      for(i=0;i<MAX_LECTURA;i++){
        linea[i]='\0';
      }
      if((leido = read(fd,linea,MAX_LECTURA)) < 0){
        perror("Read ");
        free(msg);
        close(fd);
        mq_close(queue);
        mq_unlink(nombremsg);
        exit(EXIT_FAILURE);
      }
      strcpy(msg->aviso,linea);
      if (mq_send(queue,(const char *)&msg->aviso, leido, 0) == -1) {
          fprintf(stderr, "Error sending message\n");
          free(msg);
          close(fd);
          mq_close(queue);
          mq_unlink(nombremsg);
          perror("MSG");
          return EXIT_FAILURE;
      }
      if(leido == 0){
        break;
      }
      enviados++;
    }

    msg->aviso[0]='\0';

    if (mq_send(queue, (const char *)&msg->aviso, sizeof(msg), 0) == -1) {
        fprintf(stderr, "Error sending message\n");
        free(msg);
        close(fd);
        mq_close(queue);
        mq_unlink(nombremsg);
        return EXIT_FAILURE;
    }

    enviados++;

    printf("\nHe enviado %d mensajes\n",enviados);
    free(msg);
    close(fd);
    mq_close(queue);
  exit(EXIT_SUCCESS);
  return 0;
  }
