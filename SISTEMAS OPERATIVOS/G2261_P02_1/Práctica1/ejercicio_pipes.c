/***********************************************/
/* Programa: ejercicio_pipes   Fecha:21/02/2020*/
/* Autores:Claudia González y Laura Iniesta    */
/*                                             */
/* Programa padre con 2 hijos, uno crea        */
/* un num aleatorio y lo pasa por una tuberia  */
/* el otro recibe por otra tuberia ese num     */
/* y lo imprime en un fichero                  */
/* Salida: 0: OK, -1: ERR                      */
/***********************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

/***********************************************/
/* funcion: hijo1   Fecha:21/02/2020           */
/* Autores:Claudia González y Laura Iniesta    */
/*                                             */
/* Función del hijo1 que crea el numero        */
/* aleatorio y lo escribe en la tuberia que    */
/* recibe como argumento                       */
/* Entrada: int *fd puntero a la tuberia       */
/* Salida: 0: OK, -1: ERR                      */
/***********************************************/
int hijo1(int *fd){
  int aleat=0;
  int n=10; /*Variable para determinar el max del nº aleatorio*/
  ssize_t nbytes;

  srand (time(NULL));
  aleat = rand()%(n+1);
  close(fd[0]);

 nbytes = write ( fd [1] , &aleat , sizeof(int) + 1) ;
    if ( nbytes == -1){
      perror ( " write " ) ;
      return -1 ;
    }
  close(fd[1]);
  printf("Se ha escrito el numero: %d en el pipe\n",aleat);

  return 0;
}

/***********************************************/
/* funcion: hijo2   Fecha:21/02/2020           */
/* Autores:Claudia González y Laura Iniesta    */
/*                                             */
/* Función del hijo2 que lee el numero         */
/* aleatorio y lo escribe en un fichero        */
/* Entrada: int *fd puntero a la tuberia       */
/* Salida: 0: OK, -1: ERR                      */
/***********************************************/
int hijo2(int *fd){
  int aleat;
  ssize_t nbytes = 0;
  int pf=open("numero_leido.txt",O_CREAT|O_RDWR,0644);
      if(pf==-1){
        perror("open");
        return -1;
      }

  close(fd[1]);

  nbytes = read ( fd [0] , &aleat , sizeof(int) + 1) ;
      if ( nbytes == -1){
        perror ( " read2 " ) ;
        close(pf);
        exit ( EXIT_FAILURE ) ;
      }
  if ( nbytes > 0){

    dprintf (pf,"Número : %d",aleat) ;
  }

    close(pf);
  return 0;
}

/***********************************************/
/* funcion: main   Fecha:21/02/2020            */
/* Autores:Claudia González y Laura Iniesta    */
/*                                             */
/* Crea los hijos, llama a sus funciones       */
/* y crea las tuberias pasando la informacion  */
/* recibida del hijo1 al hijo2                 */
/* Entrada: void                               */
/* Salida: 0: OK, -1: ERR                      */
/***********************************************/
int main(void) {
  int fd1[2],fd2[2];
  int flag,aleat,i;
  ssize_t nbytes = 0;
  int pipe_status1, pipe_status2;
  pid_t hijo_1, hijo_2;

    pipe_status1 = pipe(fd1);
    if(pipe_status1 == -1){
      perror("pipe1");
      exit(EXIT_FAILURE);
    }

  pipe_status2 = pipe(fd2);
  if(pipe_status2 == -1){
      perror("pipe2");
      exit(EXIT_FAILURE);
    }

  hijo_1 =fork();
    if(hijo_1 == -1){
  		perror("fork1");
  		exit(EXIT_FAILURE);
  	}

  if(hijo_1 == 0){
    flag=hijo1(fd1);
      if(flag == -1){
        perror("hijo_1");
        exit(EXIT_FAILURE);
      }

  }else{
    hijo_2 =fork();
      if(hijo_2 == -1){
        perror("fork2");
        exit(EXIT_FAILURE);
      }

    if(hijo_2 > 0){
      close(fd1[1]);

       nbytes = read ( fd1[0] , &aleat , sizeof (int)) ;
          if ( nbytes == -1){
            perror ( " read 1" ) ;
            exit ( EXIT_FAILURE ) ;
          }
        printf("El padre recibe: %d\n",aleat);
        if ( nbytes > 0){
          close(fd2[0]);

          nbytes = write ( fd2[1] , &aleat , sizeof(aleat)) ;
            if ( nbytes == -1){
              perror ( " write " ) ;
              return -1 ;
            }
            printf("Se ha pasado el num %d a la tuberia2\n",aleat);
          }
        }else if (hijo_2 ==0){
            printf("Se llama a la función del hijo 2\n");
          flag = hijo2(fd2);
            if(flag == -1){
              perror("hijo_1");
              exit(EXIT_FAILURE);
            }
        }
  }
  for(i=0;i<2;i++){
    wait(NULL);
  }
  exit(EXIT_SUCCESS);

}
