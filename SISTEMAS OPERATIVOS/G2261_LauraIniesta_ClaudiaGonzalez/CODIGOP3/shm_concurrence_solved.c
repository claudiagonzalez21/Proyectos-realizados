/**
 *
 * Descripcion: Implementacion con memoria compartida
 * entre procesos con control de concurrencia
 *
 * Fichero: shm_concurrence_solved.c
 * Autor: Claudia González Arteaga y Laura Iniesta Rodríguez
 * Version: 1.0
 * Fecha: 04-2020
 *
 */
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <tgmath.h>
#include <semaphore.h>
#include <sys/stat.h>

#define SHM_NAME "/shm_ejer"
#define SEM_NAME "/sem_ejer"
#define MAX_MSG 2000


/****************************************************/
/* Funcion: getMilClock    Fecha:04/2020            */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que calcula la hora exacta                */
/*                                                  */
/* Entrada:                                         */
/* char* buf:Puntero donde se va a almacenar        */
/* la hora calculada                                */
/****************************************************/

static void getMilClock(char *buf) {
    int millisec;
	char aux[100];
    struct tm* tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);
	millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
    if (millisec>=1000) { // Allow for rounding up to nearest second
        millisec -=1000;
        tv.tv_sec++;
    }
    tm_info = localtime(&tv.tv_sec);
    strftime(aux, 10, "%H:%M:%S", tm_info);
	sprintf(buf, "%s.%03d", aux, millisec);
}

/**
 * @brief Estructura de memoria compartida ADT.
 */
typedef struct {
	pid_t processid;       /* Logger process PID */
	long logid;            /* Id of current log line */
	char logtext[MAX_MSG]; /* Log text */
  sem_t sem;
} ClientLog;

ClientLog *shm_struct;



/****************************************************/
/* Funcion: manejador  Fecha:04/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que se ejecuta si se recibe la señal      */
/* SIGUSR1 e imprime la información del proceso     */
/* del programa                                     */
/* Entrada:                                         */
/* int sig: señal que se ha recibido                */
/****************************************************/
void manejador (int sig) {
	if (sig == SIGUSR1) {
		printf ("Log %ld: Pid %d: %s\n",shm_struct->logid, shm_struct->processid, shm_struct->logtext);
    sem_post(&shm_struct->sem);
	}else{
    sem_post(&shm_struct->sem);
    printf("No era sirgusr1");
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
/* número de procesos y número de veces que         */
/* deben ejecutar getmilclock                       */
/* Salida:                                          */
/* int: 0 con éxito -1 con error                    */
/****************************************************/
int main(int argc, char *argv[]) {
	int n, m, i, j;
  int aleat = 0;
	int ret = EXIT_FAILURE;
  int fd;
  pid_t pid;

  struct sigaction act;

  char buffer[20];

	if (argc < 3) {
		fprintf(stderr,"usage: %s <n_process> <n_logs> \n",argv[0]);
		return ret;
	}

	n = atoi(argv[1]);
	m = atoi(argv[2]);

fd=shm_open(SHM_NAME,O_RDWR | O_CREAT | O_EXCL ,S_IRUSR | S_IWUSR );


  if (fd < 0) {
      fprintf(stderr, "Error al crear la memoria compartida\n");
      return EXIT_FAILURE;
  }

  /* Resize the memory segment */
  if (ftruncate(fd, sizeof(ClientLog)) < 0) {
      fprintf(stderr, "Error al modificar el tamaño de la memoria\n");
      shm_unlink(SHM_NAME);
      return EXIT_FAILURE;
  }

  /* Map the memory segment */
  shm_struct = mmap(NULL, sizeof(*shm_struct),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

  shm_unlink(SHM_NAME);

  if (shm_struct == MAP_FAILED) {
      fprintf(stderr, "Error al mapear el segmento de memoria compartida\n");
      shm_unlink(SHM_NAME);
      return EXIT_FAILURE;
  }

  shm_struct->logid = -1;

if (sem_init(&shm_struct->sem, 1,1) < 0) {
      perror("sem_init");
      exit(EXIT_FAILURE);
    }


  sigemptyset(&(act.sa_mask));
  act.sa_flags = 0;

  /* Se arma la señal SIGUSR1. */
  act.sa_handler = manejador;
  if (sigaction(SIGUSR1, &act, NULL) < 0) {
      perror("sigaction");
      exit(EXIT_FAILURE);
  }

  for(i = 0; i < n; i++){
    pid=fork();

      if (pid < 0){
        perror("Fork ");
        exit(EXIT_FAILURE);
      }
      if(pid == 0){

        srand(time(NULL));
        for(j = 0; j < m; j++){
          aleat = rand() % 801 + 100;
          usleep(aleat);
          sem_wait(&shm_struct->sem);
          shm_struct->processid=getpid();
          shm_struct->logid= shm_struct->logid + 1;
          getMilClock(buffer);
          sprintf(shm_struct->logtext,"Soy el proceso %d a las %s",shm_struct->processid,buffer);
          if(kill(getppid(),SIGUSR1) < 0){
            printf("Error en kill\n");
            exit(EXIT_FAILURE);
          }
        }
        /* Free the shared memory*/
        sem_destroy(&shm_struct->sem);
        munmap(shm_struct, sizeof(*shm_struct));
          exit(EXIT_SUCCESS);
      }
    }

  for(j = 0; j < n*m; j++){
    sigsuspend(&(act.sa_mask));
  }


  for(i = 0; i < n; i++){
    wait(NULL);
  }

  /* Free the shared memory */
  sem_destroy(&shm_struct->sem);
  munmap(shm_struct, sizeof(*shm_struct));

	return ret;
}
