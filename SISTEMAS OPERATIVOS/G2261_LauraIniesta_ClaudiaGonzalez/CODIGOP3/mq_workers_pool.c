/**
 *
 * Descripcion: Implementacion de un programa que lee mensajes
 * a través de una cola de mensajes compartida
 *
 * Fichero: mq_workers_pool.c
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
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>

#define MAX_MESSAGE 2048

static bool fin = false;/*Variable global que indicará cuando se acaba el programa*/
static pid_t *hijos; /*Variable global para guardar los ids de los procesos hijo*/

/**
 * @brief Mensaje ADT.
 */
typedef struct {
    int valor;
    char aviso[MAX_MESSAGE];
} Mensaje;

/****************************************************/
/* Funcion: find_char      Fecha:04/2020            */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que cuenta el número de apariciones de un */
/* caracter en una cadena de texto                  */
/*                                                  */
/* Entrada:                                         */
/* int32_t msg_size: tamaño de la cadena            */
/* char* message: cadena en la que hay que buscar   */
/* char tofind: caracter que se busca               */
/* Salida:                                          */
/* int32_t caracteres: número de veces que aparece  */
/****************************************************/

static int32_t find_char(char *message, int32_t msg_size, char tofind){
  int i;
  int32_t caracteres=0;
  for(i = 0; i < msg_size; i++){
    if(message[i] == tofind){
      caracteres++;
    }
  }

  return caracteres;
}

/****************************************************/
/* Funcion: manejador_SIGTERM  Fecha:04/2020        */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que se ejecuta si se recibe la señal      */
/* SIGTERM y declara fin a true indicando el fin    */
/* del programa                                     */
/* Entrada:                                         */
/* int sig: señal que se ha recibido                */
/****************************************************/

void manejador_SIGTERM (int sig){
  fin = true;
}

/****************************************************/
/* Funcion: manejador_SIGUSR2  Fecha:04/2020        */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que se ejecuta si se recibe la señal      */
/* SIGUSR2 y envía SIGTERM a todos los procesos     */
/* del programa                                     */
/* Entrada:                                         */
/* int sig: señal que se ha recibido                */
/****************************************************/

void manejador_SIGUSR2 (int sig){
  int i=0;
  for(i=1;i<hijos[0];i++){
    kill(hijos[i],SIGTERM);
    i++;
  }
}


/****************************************************/
/* Funcion: main       Fecha:04/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que recibe los mensajes y busca un char   */
/* en los mensajes                                  */
/*                                                  */
/* Entrada:                                         */
/* int argc: Número de argumentos                   */
/* char* argv[]: argumentos                         */
/* número de procesos nombre de la cola de mensajes */
/* y caracter a buscar                              */
/* Salida:                                          */
/* int: 0 con éxito -1 con error                    */
/****************************************************/

int main(int argc, char *argv[]) {
  sigset_t sset;
  struct sigaction act,act2;
  int32_t mensajes_recibidos=0, caracteres=0, tam=0;
  int pid,N,i = 0;
  char *message;
  char tofind;
  Mensaje *msg;

  struct mq_attr attributes = {
      .mq_flags = 0,
      .mq_maxmsg = 10,
      .mq_curmsgs = 0,
      .mq_msgsize = sizeof(Mensaje)
  };

  mqd_t queue;

  if(argc != 4){
    printf("Faltan argumentos de entrada\n");
    printf("Se debe introducir el programa, el nombre del fichero y el de la cola de mensajes \n");
    exit(EXIT_FAILURE);
  }

  N = atoi(argv[1]);
  message = argv[2];
  tofind = *argv[3];

  if(N<1 || N > 10){
    printf("El numero de trabajadores tiene que estar entre 1 y 10\n");
    exit(EXIT_FAILURE);
  }

  queue = mq_open(message,O_RDONLY, 0644, &attributes);
    if (queue == (mqd_t)-1) {
        fprintf(stderr, "Error opening the queue\n");
        perror("OPENING");
        mq_unlink(message);
        return EXIT_FAILURE;
    }



    sigemptyset(&act2.sa_mask);
    act2.sa_flags = 0;
    act2.sa_handler = manejador_SIGTERM;
      /* Se arma la señal SIGTERM. */
    if (sigaction(SIGTERM, &act2, NULL) < 0) {
        perror("sigaction");
        mq_close(queue);
        mq_unlink(message);
        exit(EXIT_FAILURE);
    }

    sigemptyset(&act.sa_mask);

     act.sa_flags = 0;
     act.sa_handler = manejador_SIGUSR2;
  /* Se arma la señal SIGUSR2 */
     if (sigaction(SIGUSR2, &act, NULL) < 0) {
         perror("sigaction");
         mq_close(queue);
         mq_unlink(message);
         exit(EXIT_FAILURE);
     }
     msg = (Mensaje*)malloc(sizeof(Mensaje));
     if(!msg){
       printf("Error reservando memoria para el mensaje");
       mq_close(queue);
       mq_unlink(message);
       exit(EXIT_FAILURE);
     }

    hijos=(int*)malloc(sizeof(int));
    hijos[0]=N;

    for(i = 0; i < N; i++){
        pid=fork();

        if (pid < 0){
          perror("Fork ");
          free(msg);
          mq_close(queue);
          mq_unlink(message);
          exit(EXIT_FAILURE);
        }

        if(pid == 0){
          hijos[i+1]=getpid();

          while(fin == false){

         if ((tam = mq_receive(queue, (char*)&msg->aviso, attributes.mq_msgsize, NULL)) == -1) {
                    if(errno == EINTR)
                    goto errorvalido;

                fprintf(stderr, "Error receiving message\n");
                free(msg);
                mq_close(queue);
                mq_unlink(message);
                return EXIT_FAILURE;
            }
            if(tam > 1){
                  mensajes_recibidos++;
                  tam = strlen(msg->aviso);
              if(msg->aviso[0]=='\0'|| tam == 0){

                 if(kill(getppid(),SIGUSR2)<0){
                   perror("kill");
                   free(msg);
                   mq_close(queue);
                   mq_unlink(message);
                   return EXIT_FAILURE;
                 }
                 break;
              }else{
              caracteres=find_char(msg->aviso, tam, tofind);
              }
            }
          }
        errorvalido:  printf("\nPid hijo: %d\nMensajes leidos: %d\nCaracteres que he encontrado: %d\n", getpid(), mensajes_recibidos, caracteres);
          goto fin;
        }
      }

    sigemptyset(&sset);

   while(fin == false){
     sigsuspend(&sset);
   }



   for(i=0;i<N;i++){
     wait(NULL);
   }

   printf("Fin\n");
   fin:
   free(msg);
   free(hijos);
   mq_close(queue);
   mq_unlink(message);
   exit(EXIT_SUCCESS);
}
