#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <mqueue.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "sort.h"
#include "utils.h"

#define SHM_NAME "/MemoriaCompartidaSort"
#define MQ_NAME "/QueueCompartidaSr"
#define SECS 1
pid_t actual=0;
static int sig_sigterm = 0;
static int sig_alrm = 0;
static int sig_usr1 = 0;
static int sig_int = 0;
mqd_t queue;

  struct mq_attr attributes = {
   . mq_flags = 0 ,
   . mq_maxmsg = 10 ,
   . mq_curmsgs = 0 ,
   . mq_msgsize = sizeof ( Estado )
   };


/****************************************************/
/* Funcion: manejador  Fecha:05/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que se ejecuta si se recibe la señal      */
/* SIGTERM poniendo a 1 señal_sigterme para que     */
/* el ilustrador y los trabajadores acaben          */
/* Entrada:                                         */
/* int sig: señal que se ha recibido                */
/****************************************************/
void manejador_SIGTERM (int sig) {
  if(sig == SIGTERM){
	   sig_sigterm = 1;
  }
}
/****************************************************/
/* Funcion: manejador  Fecha:05/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que se ejecuta si se recibe la señal      */
/* SIGINT cerrando todo                             */
/* el ilustrador y los trabajadores acaben          */
/* Entrada:                                         */
/* int sig: señal que se ha recibido                */
/****************************************************/
void manejador_SIGINT (int sig) {
  if(sig == SIGINT){
	   sig_int = 1;
  }
}
/****************************************************/
/* Funcion: manejador  Fecha:05/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que se ejecuta si se recibe la señal      */
/* SIGALRM poniendo a 1 sig_alrm para que           */
/* los trabajadores escriben en tuberias            */
/* Entrada:                                         */
/* int sig: señal que se ha recibido                */
/****************************************************/
void manejador_SIGALRM(int sig) {
  if(sig == SIGALRM){
	   sig_alrm = 1;
  }

}

/****************************************************/
/* Funcion: manejador  Fecha:05/2020                */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que se ejecuta si se recibe la señal      */
/* SIGALRM poniendo a 1 sig_alrm para que           */
/* los trabajadores escriben en tuberias            */
/* Entrada:                                         */
/* int sig: señal que se ha recibido                */
/****************************************************/
void manejador_SIGUSR1(int sig) {
  if(sig == SIGUSR1){
	   sig_usr1 = 1;
  }

}

Status bubble_sort(int *vector, int n_elements, int delay, sem_t sem) {
    int i, j;
    int temp;

    if ((!(vector)) || (n_elements <= 0)) {
        return ERROR;
    }

    for (i = 0; i < n_elements - 1; i++) {
        for (j = 0; j < n_elements - i - 1; j++) {
            /* Delay. */
            fast_sleep(delay);
            if (vector[j] > vector[j+1]) {
                temp = vector[j];
                vector[j] = vector[j + 1];
                vector[j + 1] = temp;
            }
        }
    }

    return OK;
}

Status merge(int *vector, int middle, int n_elements, int delay,sem_t sem) {
    int *aux = NULL;
    int i, j, k, l, m;

    if (!(aux = (int *)malloc(n_elements * sizeof(int)))) {
        return ERROR;
    }

    for (i = 0; i < n_elements; i++) {
        aux[i] = vector[i];
    }

    i = 0; j = middle;
    for (k = 0; k < n_elements; k++) {
        /* Delay. */
        fast_sleep(delay);
        if ((i < middle) && ((j >= n_elements) || (aux[i] < aux[j]))){
            vector[k] = aux[i];
            i++;
        }
        else {
            vector[k] = aux[j];
            j++;
        }

        /* This part is not needed, and it is computationally expensive, but
        it allows to visualize a partial mixture. */
        m = k + 1;
        for (l = i; l < middle; l++) {
            vector[m] = aux[l];
            m++;
        }
        for (l = j; l < n_elements; l++) {
            vector[m] = aux[l];
            m++;
        }
    }

    free((void *)aux);

    return OK;
}

int get_number_parts(int level, int n_levels) {
    /* The number of parts is 2^(n_levels - 1 - level). */
    return 1 << (n_levels - 1 - level);
}

Status init_sort(char *file_name, Sort *sort, int n_levels, int n_processes, int delay) {
    char string[MAX_STRING];
    FILE *file = NULL;
    int i, j, log_data;
    int block_size, modulus;


    if ((!(file_name)) || (!(sort))) {
        fprintf(stderr, "init_sort - Incorrect arguments\n");
        return ERROR;
    }
    /* At most MAX_LEVELS levels. */
    sort->n_levels = MAX(1, MIN(n_levels, MAX_LEVELS));
    /* At most MAX_PARTS processes can work together. */
    sort->n_processes = MAX(1, MIN(n_processes, MAX_PARTS));
    /* The main process PID is stored. */
    sort->ppid = getpid();
    /* Delay for the algorithm in ns (less than 1s). */
    sort->delay = MAX(1, MIN(999999999, delay));

    if (!(file = fopen(file_name, "r"))) {
        perror("init_sort - fopen");
        return ERROR;
    }

    /* The first line contains the size of the data, truncated to MAX_DATA. */
    if (!(fgets(string, MAX_STRING, file))) {
        fprintf(stderr, "init_sort - Error reading file\n");
        fclose(file);
        return ERROR;
    }
    sort->n_elements = atoi(string);
    if (sort->n_elements > MAX_DATA) {
        sort->n_elements = MAX_DATA;
    }

    /* The remaining lines contains one integer number each. */
    for (i = 0; i < sort->n_elements; i++) {
        if (!(fgets(string, MAX_STRING, file))) {
            fprintf(stderr, "init_sort - Error reading file\n");
            fclose(file);
            return ERROR;
        }
        sort->data[i] = atoi(string);
    }
    fclose(file);

    /* Each task should have at least one element. */
    log_data = compute_log(sort->n_elements);
    if (n_levels > log_data) {
        n_levels = log_data;
    }
    sort->n_levels = n_levels;

    /* The data is divided between the tasks, which are also initialized. */
    block_size = sort->n_elements / get_number_parts(0, sort->n_levels);
    modulus = sort->n_elements % get_number_parts(0, sort->n_levels);
    sort->tasks[0][0].completed = INCOMPLETE;
    sort->tasks[0][0].ini = 0;
    sort->tasks[0][0].end = block_size + (modulus > 0);
    sort->tasks[0][0].mid = NO_MID;
    for (j = 1; j < get_number_parts(0, sort->n_levels); j++) {
        sort->tasks[0][j].completed = INCOMPLETE;
        sort->tasks[0][j].ini = sort->tasks[0][j - 1].end;
        sort->tasks[0][j].end = sort->tasks[0][j].ini \
            + block_size + (modulus > j);
        sort->tasks[0][j].mid = NO_MID;
    }
    for (i = 1; i < n_levels; i++) {
        for (j = 0; j < get_number_parts(i, sort->n_levels); j++) {
            sort->tasks[i][j].completed = INCOMPLETE;
            sort->tasks[i][j].ini = sort->tasks[i - 1][2 * j].ini;
            sort->tasks[i][j].mid = sort->tasks[i - 1][2 * j].end;
            sort->tasks[i][j].end = sort->tasks[i - 1][2 * j + 1].end;
        }
    }
    return OK;
}

Bool check_task_ready(Sort *sort, int level, int part) {
  sem_wait(&(sort->sem));
    if (!(sort)) {
        return FALSE;
    }

    if ((level < 0) || (level >= sort->n_levels) \
        || (part < 0) || (part >= get_number_parts(level, sort->n_levels))) {
        return FALSE;
    }

    if (sort->tasks[level][part].completed != INCOMPLETE) {
        return FALSE;
    }

    /* The tasks of the first level are always ready. */
    if (level == 0) {
        return TRUE;
    }

    /* Other tasks depend on the hierarchy. */
    if ((sort->tasks[level - 1][2 * part].completed == COMPLETED) && \
        (sort->tasks[level - 1][2 * part + 1].completed == COMPLETED)) {
        return TRUE;
    }
sem_post(&(sort->sem));
    return FALSE;
}

Status solve_task(Sort *sort, int level, int part) {


    /* In the first level, bubble-sort. */
    if (sort->tasks[level][part].mid == NO_MID) {
        return bubble_sort(\
            sort->data + sort->tasks[level][part].ini, \
            sort->tasks[level][part].end - sort->tasks[level][part].ini, \
            sort->delay,sort->sem);
    }
    /* In other levels, merge. */
    else {
        return merge(\
            sort->data + sort->tasks[level][part].ini, \
            sort->tasks[level][part].mid - sort->tasks[level][part].ini, \
            sort->tasks[level][part].end - sort->tasks[level][part].ini, \
            sort->delay,sort->sem);
    }


}

Status sort_multi_process(char *file_name, int n_levels, int n_processes, int delay) {
    int i, j,aux;
    Sort *sort ;
    int fd_shm;
    pid_t hijos[MAX_PARTS];
    pid_t pid, ilust;
    int ilus_trab[MAX_PARTS][2],trab_ilus[MAX_PARTS][2];
    /*int pipe_status;*/
    struct sigaction act,act2,act3;
    sigset_t mask;
    Estado msg ;

    fd_shm =shm_open(SHM_NAME,O_RDWR | O_CREAT | O_EXCL ,S_IRUSR | S_IWUSR );

        if (fd_shm < 0) {
              fprintf(stderr, "Error abriendo el segmento de memoria compartida\n");
                exit(EXIT_FAILURE);
          }


        if (ftruncate(fd_shm, sizeof(Sort)) < 0) {
            fprintf(stderr, "Error al redimensionar los segmen de memoria compartida\n");
            shm_unlink(SHM_NAME);
            exit(EXIT_FAILURE);
        }

        sort = mmap(NULL, sizeof(Sort), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

        if (sort == MAP_FAILED) {
            fprintf(stderr, "Error al mapear el segmento de memoria compartida\n");
            munmap ((void*) sort , sizeof (Sort)) ;
            shm_unlink(SHM_NAME);
            close(fd_shm);
            exit(EXIT_FAILURE);
        }
          close(fd_shm);

          if (sem_init(&sort->sem, 1,1) < 0) {
                perror("sem_init");
                munmap ((void*) sort , sizeof (Sort)) ;
                shm_unlink(SHM_NAME);
                free(sort);

                exit(EXIT_FAILURE);
          }
          shm_unlink(SHM_NAME);
    /* The data is loaded and the structure initialized. */
    if (init_sort(file_name, sort, n_levels, n_processes, delay) == ERROR) {
        fprintf(stderr, "sort_single_process - init_sort\n");
        sem_destroy(&sort->sem);
        munmap ((void*) sort , sizeof (Sort)) ;
        free(sort);
        shm_unlink(SHM_NAME);
        return ERROR;
    }



    queue = mq_open ( MQ_NAME , O_CREAT |O_WRONLY|O_RDONLY ,S_IRUSR | S_IWUSR,&attributes);

     if ( queue == ( mqd_t ) -1) {
      sem_destroy(&sort->sem);
       munmap ((void*) sort , sizeof (Sort)) ;
       return EXIT_FAILURE ;
     }



     sigemptyset(&(act.sa_mask));
     act.sa_flags = 0;

     /* Se arma la señal SIGALRM.*/
     act.sa_handler = manejador_SIGTERM;
     if (sigaction(SIGTERM, &act, NULL) < 0) {
         perror("sigaction");
         sem_destroy(&sort->sem);
         munmap ((void*) sort , sizeof (Sort)) ;
         shm_unlink(SHM_NAME);
         sem_destroy(&sort->sem);
         exit(EXIT_FAILURE);
     }

     sigemptyset(&(act3.sa_mask));
     act.sa_flags = 0;

     /*Se arma la señal SIGALRM.*/
    act.sa_handler = manejador_SIGINT;
     if (sigaction(SIGINT, &act3, NULL) < 0) {
         perror("sigaction");
         sem_destroy(&sort->sem);
         munmap ((void*) sort , sizeof (Sort)) ;
         shm_unlink(SHM_NAME);
         sem_destroy(&sort->sem);
         exit(EXIT_FAILURE);
     }

     sigemptyset(&(act2.sa_mask));
     act2.sa_flags = 0;

     /* Se arma la señal SIGALRM. */
    act2.sa_handler = manejador_SIGUSR1;
     if (sigaction(SIGUSR1, &act2, NULL) < 0) {
         perror("sigaction");
         sem_destroy(&sort->sem);
         munmap ((void*) sort , sizeof (Sort)) ;
         shm_unlink(SHM_NAME);
         sem_destroy(&sort->sem);
         exit(EXIT_FAILURE);
     }

      sigemptyset (&mask);
      sigaddset (&mask, SIGUSR1);
      sigprocmask (SIG_BLOCK, &mask, &act2.sa_mask);


/*INICIALIZAMOS PIPES ILUS_TRAB el ilustrador escribe a los TRABAJADORES
trab_ilus el trabajador escribe a los ilustradores*/
/*for(i = 0; i< (sort->n_levels); i++){
  for(j=0;j<get_number_parts(i, sort->n_levels);i++){
    pipe_status = pipe(ilus_trab[i]);
    if(pipe_status == -1){
      perror("pipe1");
      sem_destroy(&sort->sem);
       munmap ((void*) sort , sizeof (Sort)) ;
       shm_unlink(SHM_NAME);
      exit(EXIT_FAILURE);
    }
    pipe_status = pipe(trab_ilus[i]);
    if(pipe_status == -1){
      perror("pipe2");
      sem_destroy(&sort->sem);
       munmap ((void*) sort , sizeof (Sort)) ;
       shm_unlink(SHM_NAME);
      exit(EXIT_FAILURE);
    }
  }
}*/
/*Preparamos la funcion pintar*/
plot_vector(sort->data, sort->n_elements);
printf("\nStarting algorithm with %d levels and %d processes...\n", sort->n_levels, sort->n_processes);

/*CREAMOS EL ILUSTRADOR*/
  pid = fork();
  if(pid < 0){
    perror("Fork ");
    sem_destroy(&sort->sem);
    munmap ((void*) sort , sizeof (Sort)) ;
    shm_unlink(SHM_NAME);
    exit(EXIT_FAILURE);
  }
  if(pid == 0){
    ilust = getpid();
    if(ilustrador(sort,ilust,trab_ilus, ilus_trab)== ERROR){
      sem_destroy(&sort->sem);
      munmap ((void*) sort , sizeof (Sort)) ;
      shm_unlink(SHM_NAME);
      exit(EXIT_FAILURE);
    }
  /*  HIJO TIENE QUE IMPRIMIR INFO DE TODOS; CREAR FUNCION*/
}

/*CREAMOS A LOS TRABAJADORES*/
  for(i=0;i<MAX_PARTS;i++){
    pid = fork();
    if(pid < 0){
      perror("Fork ");
      sem_destroy(&sort->sem);
      munmap ((void*) sort , sizeof (Sort)) ;
      shm_unlink(SHM_NAME);
      mq_close(queue);
      exit(EXIT_FAILURE);
    }

    if(pid == 0){
      hijos[i]=getpid();
      actual = hijos[i];
      if(trabajador(sort,hijos[i],trab_ilus[i],ilus_trab[i]) == ERROR){
        sem_destroy(&sort->sem);
        munmap ((void*) sort , sizeof (Sort)) ;
        shm_unlink(SHM_NAME);
        mq_close(queue);
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);
    }
  }
  printf("\nVAMOS A ENVIAR TASKS \n");
  /* For each level, and each part, the corresponding task is solved. */
  for (i = 0; i < sort->n_levels; i++) {
    int max = get_number_parts(i, sort->n_levels);

      for (j = 0; j < get_number_parts(i, sort->n_levels); j++) {
        max--;
        if(sig_int == 1){
              goto fin;
        }
        msg.level = i;
        msg.parte = j;
        if ((mq_send(queue,(const char *)&msg, sizeof(msg),max )) == -1) {
          if(errno != EINTR){
            fprintf(stderr, "Error sending message\n");
            mq_close(queue);
            sem_destroy(&sort->sem);
            munmap ((void*) sort , sizeof (Sort)) ;
            shm_unlink(SHM_NAME);
            perror("MSG");
            return EXIT_FAILURE;
          }
        }
        sem_wait(&(sort->sem));
        sort->tasks[i][j].completed = SENT;
        sem_post(&(sort->sem));

      }
      /*ESPERAR AA SIGUSR1*/
      while (sig_usr1 !=1 && sig_int!=1){
        sigsuspend (&act2.sa_mask); /*REVISAR LA MASCARA QUE PASO*/
      }
        sem_wait(&(sort->sem));
        for(j = 0; j < get_number_parts(i, sort->n_levels); j++){
          if(sort->tasks[i][j].completed == COMPLETED){
            printf("\nCompletado nivel : %d parte: %d\n",i,j);
            aux++;
          }
        sig_usr1 = 0;
        if(aux == max){
          printf("\nNivel: %d COMPLETO\n",i);
          break;
        }
        sem_post(&(sort->sem));
      }
  }

/*FINALIZAMOS LOS HIJOS*/
  fin:
  kill(ilust,SIGTERM);
  for(i = 0; i< MAX_PARTS;i++){
    kill(hijos[i],SIGTERM);
  }

  plot_vector(sort->data, sort->n_elements);

    for(i = 0; i < MAX_PARTS; i++){
      wait(NULL);
    }
    sem_destroy(&sort->sem);
    munmap ((void*) &sort , sizeof (Sort)) ;
    shm_unlink(SHM_NAME);
    printf("\nAlgorithm completed\n");

    return OK;
}

int ilustrador(Sort *sort,pid_t id, int trab_ilus[MAX_PARTS][2], int ilus_trab[MAX_PARTS][2]){
    Estado estado[MAX_PARTS];
    int i;
    /*int fin = 0,nbytes;*/ /*SI es 1 han acabado si es 0 no*/
    /*LEEMOS EL ESTADO DE CADA TRABAJADOR*/
  while(sig_sigterm == 0){
    for(i=0;i < MAX_PARTS;i++){
      estado[i].level = -1;
      estado[i].parte = -1;
      estado[i].proceso = -1;
    /* close(trab_ilus[i][1]);
      nbytes = read ( trab_ilus[i][0] , &estado[i] , sizeof (Estado)) ;
         if ( nbytes == -1&& errno != EINTR){
           perror ( " read 1" ) ;
           return ERROR;
         }
         if(nbytes > 0){
           fin = 1;
         }*/
      }

/**IMPRIMIMOS LOS DATOS*/
 plot_vector(sort->data, sort->n_elements);
  printf("\n%10s%10s%10s%10s%10s\n", "PID", "LEVEL", "PART", "INI", \
      "END");
  printf("%10d%10d%10d%10d%10d\n", estado->proceso, estado->level, estado->parte, \
      sort->tasks[estado->level][estado->parte].ini, sort->tasks[estado->level][estado->parte].end);
   for(i=0;i<MAX_PARTS;i++){
      printf("Proceso %d trabaja en el nivel %d en la parte %d",estado[i].proceso, estado[i].level,estado[i].parte);
    }

/*
  for(i =0;i< MAX_PARTS;i++){
        close(ilus_trab[i][0]);
         nbytes = write ( ilus_trab[i][1] , &fin , sizeof(int)) ;
           if ( nbytes == -1 && errno != EINTR){
             perror ( " write1 " ) ;
             return ERROR ;
           }
  }*/
}

/*CERRAMOS LAS TUBERIAS CREO QUE VA EN EL MAIN*/
  /*  for(i=0;i<MAX_PARTS;i++){
      close(trab_ilus[i][0]);
      close(trab_ilus[i][1]);
      close(ilus_trab[i][1]);
      close(ilus_trab[i][0]);
    }*/

    return OK;
  }

int trabajador (Sort *sort,pid_t id,int trab_ilus[2], int ilus_trab[2]){
    int tam=0;
    /*int nbytes,aux,j,i;*/
    Estado msg;
    Estado estado;
    struct sigaction act;
    char* estado_task="";
    sigset_t mask;

    msg.parte=0;
    msg.level=0;
    msg.proceso = id;

    /*CERRAR PIPES EXTREMOS CORRESPONDIENTES*/
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;

     /*Se arma la señal SIGALRM.*/
    act.sa_handler = manejador_SIGALRM;
    if (sigaction(SIGALRM, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sigemptyset (&mask);
    sigaddset (&mask, SIGALRM);
    sigprocmask (SIG_BLOCK, &mask, &act.sa_mask);

    queue = mq_open ( MQ_NAME, O_RDONLY ,S_IRUSR | S_IWUSR,&attributes ) ;
     if ( queue == ( mqd_t ) -1) {
      sem_destroy(&sort->sem);
       munmap ((void*) sort , sizeof (Sort)) ;
       return EXIT_FAILURE ;
     }

/*RECIBE UN TASK*/
  while(sig_sigterm == 0){
     alarm(SECS);
    while(sigsuspend(&act.sa_mask)){

      if ((tam = mq_receive(queue, (char*)&msg, sizeof(Estado), NULL)) < 0) {
            if(errno != EINTR){
            perror("Mensaje");
            fprintf(stderr, "Error receiving message\n");

            return ERROR;
          }
        }
        sem_wait(&(sort->sem));
         if(tam > 0){
          sort->tasks[msg.level][msg.parte].completed = PROCESSING;
          estado_task = "En proceso";
          if(solve_task(sort, msg.level,msg.parte)== ERROR){
              sort->tasks[msg.level][msg.parte].completed = INCOMPLETE;
              estado_task = "Incompleto";
          }
          sort->tasks[msg.level][msg.parte].completed = COMPLETED;
          estado_task = "Completo";
          actual = -1;
          kill(getppid(),SIGUSR1);
         }
          sem_post (&sort->sem);
        if(sig_alrm == 1){
          sig_alrm = 0;
          estado.parte = msg.parte;
          estado.level = msg.level;
          estado.estado_task = estado_task;
          break;
        }
    }
    sig_alrm = 0;
  /*  estado.parte = msg.parte;
    estado.level = msg.level;
    estado.estado_task = estado_task;*/

   /*close(trab_ilus[0]);
    nbytes = write ( trab_ilus[1] , &estado, sizeof(Estado));
     if ( nbytes == -1&& errno != EINTR){
       perror ( " write 2" ) ;
       return ERROR ;
     }

     while(aux !=1){
       close(ilus_trab[1]);
       nbytes = read (ilus_trab[0] , &aux, sizeof(int));
        if ( nbytes == -1&& errno != EINTR){
          perror ( " read 2 " ) ;
          return ERROR ;
        }

    }*/


  }

    mq_close(queue);
    return OK;
}
