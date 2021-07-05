/*
** Fichero: ejercicio_lect_escr.c
** Autor: Claudia Gonzalez Arteaga y Laura Iniesta Rodriguez
** Correo: claudia.gonzaleza@estudiante.uam.es y laura.iniesta@estudiante.uam.es
** Grupo: 2261
** Fecha: 21-03-2020
** Descripcion: programa que implementa el algoritmo de lectores–escritores
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>



#define N_READ 10
#define SECS 0
#define SEM_READ_NAME "/read"
#define SEM_WRITE_NAME "/write"
#define SEM_NREADERS_NAME "/nreaders"

static sem_t * sem_read = NULL;
static sem_t * sem_write = NULL;
static sem_t * sem_nreaders = NULL;

static pid_t pid_hijos[N_READ] = {0};

int getvalue(sem_t * sem){
  int v;

  if (sem_getvalue(sem, &v) == -1) {
		perror("sem_getvalue");
		exit(EXIT_FAILURE);
	}

  return v;
}

void manejadorSigint(int signal){
  int i;

  for (i = 0; i < getvalue(sem_nreaders); i++) {
      if(kill(pid_hijos[i], SIGTERM)== -1){
        perror("error");
        exit(EXIT_FAILURE);
      }
    }

  for(i = 0; i < getvalue(sem_nreaders); i++){
      wait(NULL);
  }
  exit(EXIT_SUCCESS);
}

void set_handler(int signal, void (*handler)(int)){
  struct sigaction act;
  int i;

  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = handler;

  if (sigaction(signal, &act, NULL) < 0) {
      perror("sigaction");
      exit(EXIT_FAILURE);
      for(i=0; i<getvalue(sem_nreaders); i++){
        if(pid_hijos[i] != 0){
              kill(pid_hijos[i], SIGTERM);
              wait(NULL);
        }
      }
  }

}

void down(sem_t * sem){

  if(sem_wait(sem) < 0){
     perror("sem_wait down " );
     sem_close(sem);
   }
}

void up(sem_t * sem){
  if(sem_post(sem) < 0){
     perror("sem_wait post " );
     sem_close(sem);
   }
}


void read_body(void){

    fprintf(stdout, "R-INI <%d>\n", getpid());
    sleep(1);
    fprintf(stdout, "R-fin <%d>\n", getpid());
}

void write_body(void){

  printf("W-INI <%d>\n", getpid());
  sleep(1);
  printf("W-fin <%d>\n", getpid());
}


void protected_read (void) {

  down(sem_read);
  up(sem_nreaders);
  if (getvalue(sem_nreaders) == 1){
      down(sem_write);
  }
  up(sem_read);
  read_body();
  down (sem_read) ;
  down(sem_nreaders);
  if (getvalue(sem_nreaders) == 0){
      up(sem_write);
  }
  up (sem_read);
}



void protected_write(void){

  down(sem_write);
  write_body();
  up(sem_write);
}

int main(void){
     int i, pid, ppid;

     set_handler(SIGINT, SIG_IGN);


     ppid=getpid();

     if ((sem_read = sem_open(SEM_READ_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
       perror("sem_open");
       exit(EXIT_FAILURE);
     }

     up(sem_read);
     sem_unlink(SEM_READ_NAME);

     if ((sem_write = sem_open(SEM_WRITE_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
       perror("sem_open");
       exit(EXIT_FAILURE);
     }

     up(sem_write);
     sem_unlink(SEM_WRITE_NAME);

     if ((sem_nreaders = sem_open(SEM_NREADERS_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
       perror("sem_open");
       exit(EXIT_FAILURE);
     }
     sem_unlink(SEM_NREADERS_NAME);


    for(i=0; i<N_READ; i++){
      pid=fork();
      if(!pid){
        pid_hijos[i]= getpid();
        while(1){
          protected_read();
          sleep(SECS);
        }
      }
    }

      set_handler(SIGINT, manejadorSigint);

      while(1){
        protected_write();
        sleep(SECS);
      }


}
