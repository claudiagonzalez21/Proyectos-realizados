/*
** Fichero: ejercicio_prottemp_mejorado.c
** Autor: Claudia Gonzalez Arteaga y Laura Iniesta Rodriguez
** Correo: claudia.gonzaleza@estudiante.uam.es y laura.iniesta@estudiante.uam.es
** Grupo: 2261
** Fecha: 21-03-2020
** Descripcion: programa ejercicio_prottemp.c mejorado:
** el proceso padre lee de un fichero el resultado final del trabajo ejecutado por los hijos y pueda imprimirlo
** en pantalla.
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>

#define SIZE  100
#define SEM_NAME  "/sem_ej12"

static volatile int tiempo_padre = 0;
static volatile int term = 0;
static volatile int fin = 0;

void manejadorSigterm(int signal){
    term=1;
}
void manejadorAlarma(int signal){
  tiempo_padre=1;
}
void manejadorSigusr2 (int signal){
  fin = 1;
}


int escribe_fichero(int fd, int num_proc, long int suma) {
  char linea[SIZE];
  size_t num_car;
  memset((void*)linea,0,SIZE);

    printf("LO QUE VA A ESCRIBIR\n%d\n%ld\n",num_proc,suma );

  if((num_car = sprintf(linea,"%d\n%ld",num_proc,suma)) < 2){
    printf("Problema escribiendo en la variable\n");
    return -1;
  }
  if( lseek(fd,0,SEEK_SET) < 0){
    perror("lseek ");
    exit(EXIT_FAILURE);
  }
  if(write(fd,linea,num_car) < num_car){
    perror ("Write ");
    return -1;
  }
  printf("\nLO QUE HA ESCRITO \n%s \n",linea);

  if( lseek(fd,0,SEEK_SET) < 0){
    perror("lseek ");
    return -1;
  }
  return 0;
}


int hijo(int pid_padre, sigset_t * oldm, sem_t * sem) {
    pid_t pid;
    int i, num_proc = 0;
    int fd = 0;
    long int suma = 0, sum = 0;
    double div;
    char linea[SIZE];
    pid = getpid();
    div = pid/10;

    memset((void*)linea,0,SIZE);

    for(i = 1;i <= div; i++){
      sum+=i;
    }
    printf("El hijo con PID = %d suma: %.1ld \n", pid, sum);

    fd = open("datos.txt",O_RDWR ,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
      if(fd < 0){
        perror("Open ");
        return -1;
      }
     if(sem_wait(sem) < 0){
        perror("sem_wait2 " );
        sem_close(sem);
        return -1;
      }

      if( lseek(fd,0,SEEK_SET) < 0){
        perror("lseek ");
        return -1;
      }

      if(read(fd,&linea[0],10) < 0){
        perror("Read ");
        return -1;
      }

      if( read(fd,&linea[1],20) < 0){
        perror("Read ");
        return -1;
      }

        printf("\nLO LEIDO: %s por PID: %d\n\n",linea,pid );

      sscanf(&linea[0],"%d",&num_proc);
      sscanf(&linea[1],"%ld",&suma);

    suma+=sum;
    num_proc ++;

    printf("\nLO CALCULADO\n%d\n%ld\n\n",num_proc,suma );

    if( lseek(fd,0,SEEK_SET) < 0){
      perror("lseek ");
      return -1;
    }

    if(escribe_fichero(fd,num_proc,suma) < 0){
        printf("Error en escribe_fichero\n");
      return -1;
    }
    close(fd);

    if(sem_post(sem) < 0){
        perror("Sem_post1 ");
        sem_close(sem);
        return -1;
    }


    if(kill(pid_padre, SIGUSR2)!=0){
      perror("KILL");
      exit(EXIT_FAILURE);
    }

    printf("PID: %d ha enviado sigusr2 al padre\n",pid);

    while(term != 0){
      sigsuspend(oldm);
    }
    term = 0;
    printf("PID: %d TERMINADO\n",pid);
    exit(EXIT_SUCCESS);
}

int main(int argc, char * argv[]){

    int n = 0,t = 0, i,j, num_proc = 0;
    long int suma = 0;
    pid_t *pid_hijos;
    pid_t pid;
    int pid_padre;
    int fd;
    sigset_t padrem, oldm;
    struct sigaction act;
    char linea[SIZE];
    sem_t * sem = NULL;

//Input parsing

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

      fd = creat("datos.txt",0644);
      if(escribe_fichero(fd, num_proc, suma) < 0){
        perror("Write ");
        exit(EXIT_FAILURE);
      }
      close(fd);

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

    if ((sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
      perror("sem_open");
      exit(EXIT_FAILURE);
    }
      sem_unlink(SEM_NAME);
        if(sem_post(sem) < 0){
          perror("Sem_post2 ");
          sem_close(sem);
          exit(EXIT_FAILURE);
        }


    pid_hijos = (int*)malloc(n*sizeof(int));

      if(!pid_hijos){
        exit(EXIT_FAILURE);
      }

  for(i = 0; i < n; i++){

    pid=fork();

      if (pid < 0){
        perror("Fork ");
        exit(EXIT_FAILURE);
      }
      if(pid == 0){
          pid_hijos[i]=getpid();
        if(hijo(getppid(),&oldm,sem) < 0){
          close(fd);
         sem_close(sem);
         free(pid_hijos);
          exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
      }
    }

      if (alarm(t)) {
          fprintf(stderr, "Existe una alarma previa establecida\n");
          free(pid_hijos);
          exit(EXIT_FAILURE);
      }



      fd = open("datos.txt",O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
        if(fd < 0){
          perror("Open ");
          return -1;
        }

      while(!tiempo_padre){
        sigsuspend(&oldm);

        if(sem_wait(sem) < 0){
          perror("sem_wait1 " );
          sem_close(sem);
          exit(EXIT_FAILURE);
        }

        if(fin == 1){

            if( lseek(fd,0,SEEK_SET) < 0){
              perror("lseek ");
              free(pid_hijos);
              exit(EXIT_FAILURE);
            }
            if(read(fd,&linea[0],10) < 0){
              perror("Read ");
              free(pid_hijos);
              exit(EXIT_FAILURE);
            }

            if( read(fd,&linea[1],30) < 0){
              perror("Read ");
              free(pid_hijos);
              exit(EXIT_FAILURE);
            }

            sscanf(&linea[0],"%d",&num_proc);
            sscanf(&linea[1],"%ld",&suma);

            if(sem_post(sem) < 0){
              perror("Sem_post3 ");
              sem_close(sem);
              exit(EXIT_FAILURE);
            }

            if(num_proc == n){
              printf("Han acabado todos, el resultado obtenido es: %ld\n",suma);

              break;
            }

            fin = 0;
        }

      }

      if(tiempo_padre == 1){
        printf("Falta trabajo, numero de procesos ejecutados: %d\n", num_proc);
      }
      close(fd);

      for (j = 0; j < n; j++) {
          kill(pid_hijos[j], SIGTERM);
        }

      for(i=0;i< n;i++){
          wait(NULL);
      }


      if(sigprocmask(SIG_SETMASK,&padrem,NULL)<0){
        printf("ERROR en sigprocmask");
        exit(EXIT_FAILURE);
      }

    printf("El programa ha finalizado\n");


    exit(EXIT_SUCCESS);
}
