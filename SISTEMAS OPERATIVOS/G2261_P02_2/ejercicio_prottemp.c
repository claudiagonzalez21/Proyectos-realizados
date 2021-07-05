/*
** Fichero: ejercicio_prottemp.c
** Autor: Claudia Gonzalez Arteaga y Laura Iniesta Rodriguez
** Correo: claudia.gonzaleza@estudiante.uam.es y laura.iniesta@estudiante.uam.es
** Grupo: 2261
** Fecha: 21-03-2020
** Descripcion: programa ejercicio.c que cumple los requisitos indicados
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

static volatile int num_s = 0;
static volatile int tiempo_padre = 0;
static volatile int term = 0;

void manejadorSigterm(int signal){
    term=1;
}
void manejadorAlarma(int signal){
  tiempo_padre=1;
}
void manejadorSigusr2 (int signal){

  num_s++;
  printf("\n%d",num_s);
}

int hijo(int pid_padre, sigset_t * oldm){
  pid_t pid;
  int i;
  double sum = 0, div;
  pid = getpid();
  div = pid/10;

  for(i = 1;i <= div; i++){
    sum+=i;
  }
  printf("El hijo con PID = %d suma: %.1lf \n", pid, sum);

  kill(pid_padre, SIGUSR2);

  printf("Se ha enviado sigusr2 al padre\n");

  while(term != 0){
    sigsuspend(oldm);
  }
  term =0;
printf("PID: %d TERMINADO\n",pid);
  exit(EXIT_SUCCESS);

}

int main(int argc , char * argv[] ){
  int n = 0,t = 0, i,j;
  pid_t *pid_hijos;
  pid_t pid;
  int pid_padre;
  sigset_t padrem, oldm;
  struct sigaction act;



  if(argc != 3){
    printf("Faltan argumentos de entrada\n");
    printf("Se debe introducir el programa, el número de hijos y el tiempo de espera \n");
      exit(EXIT_FAILURE);
  }
  n = atoi(argv[1]);
  t = atoi(argv[2]);

  if(n < 1 || t < 1){
    printf("Error con el tamaño de los argumentos de entrada");
    exit(EXIT_FAILURE);
  }

  pid_padre = getpid();

  sigemptyset(&padrem);
  sigaddset(&padrem, SIGALRM);

  if(sigprocmask(SIG_BLOCK,&padrem,&oldm)<0){
    printf("ERROR en sigprocmask");
    exit(EXIT_FAILURE);
  }

  act.sa_flags = 0;
  act.sa_handler = manejadorAlarma;
  act.sa_mask = padrem;
    /* Se arma la señal SIGALRM. */
  if (sigaction(SIGALRM, &act, NULL) < 0) {
      perror("sigaction");
      exit(EXIT_FAILURE);
  }


     act.sa_flags = SA_NODEFER;
     act.sa_handler = manejadorSigusr2;
  /* Se arma la señal SIGUSR2 */
     if (sigaction(SIGUSR2, &act, NULL) < 0) {
         perror("sigaction");
         exit(EXIT_FAILURE);
     }

        act.sa_flags = 0;
        act.sa_handler = manejadorSigterm;
          /* Se arma la señal SIGTERM. */
        if (sigaction(SIGTERM, &act, NULL) < 0) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
      pid_hijos = (int*)malloc(n*sizeof(int));

        if(!pid_hijos){
          exit(EXIT_FAILURE);
        }

    for(i=0;i<n;i++){

      pid=fork();

        if (pid < 0){
          perror("Fork ");
          exit(EXIT_FAILURE);
        }
        if(pid == 0){
          hijo(getppid(),&oldm);
          pid_hijos[i]=getpid();
          exit(EXIT_SUCCESS);
        }

    }

      if (alarm(t)) {
          fprintf(stderr, "Existe una alarma previa establecida\n");
      }

      while(!tiempo_padre){
        sigsuspend(&oldm);
      }

      for (j = 0; j < n; j++) {
          kill(pid_hijos[j], SIGTERM);
        }
      for(i = 0;i < n;i++){
          wait(NULL);
      }


    if(sigprocmask(SIG_SETMASK,&padrem,NULL)<0){
      printf("ERROR en sigprocmask");
      exit(EXIT_FAILURE);
    }

    printf("\nSe han recibido %d señales\n\n",num_s);
    printf("El programa ha finalizado\n");

    exit(EXIT_SUCCESS);

}
