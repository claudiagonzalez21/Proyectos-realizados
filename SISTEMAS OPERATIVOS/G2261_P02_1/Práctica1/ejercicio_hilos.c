#include <errno.h>

#include <pthread.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>



typedef struct {
    int tiempo;
    int x;
} estructura;



void *cubo(void *arg) {

    estructura *est=arg;
		int *respuesta;

		sleep(est->tiempo);


		respuesta = (int*)malloc(sizeof(respuesta));
		if(!respuesta){
			printf("Error. No se ha podido reservar memoria para el vector respuesta.");
			exit(EXIT_FAILURE);
		}

		*respuesta = (est->x)*(est->x)*(est->x);

		pthread_exit(respuesta);


}

int main(int argc, char *argv[]) {

    estructura *es;
		pthread_t hilo;
		int i;
		int n_hilos;
		int *respuesta;
		int error;


		if(argc != 2){
			printf("Error. Número incorrecto de argumentos. \n");
			return -1;
		}


	 n_hilos=atoi(argv[1]);


	 respuesta = (int*)malloc(n_hilos*sizeof(int));

	 if(!respuesta){
		 printf("Error. No se ha podido reservar memoria para el vector respuesta.");
		 exit(EXIT_FAILURE);
	 }

	 es = (estructura*)malloc(n_hilos*sizeof(estructura));

	 if(!es){
		 printf("Error. No se ha podido reservar memoria para el vector estructura.");
		 free(respuesta);
		 exit(EXIT_FAILURE);
	 }

		hilo = (pthread_t)malloc(n_hilos*(sizeof(pthread_t)));
		if(!hilo){
			printf("Error. No se ha podido reservar memoria para el vector hilos.");
			free(respuesta);
			free(es);
			exit(EXIT_FAILURE);
		}

		for(i=1; i<=n_hilos; i++){
			srand(time(NULL));

			es->tiempo=rand()%10;
			es->x=i;

			error=pthread_create(&hilo, NULL, cubo, es);

			if(error != 0)
			{
				/*fprintf(stderr, "pthread_create: %s\n", perror(error));*/
        free(respuesta);
        free(es);
				exit(EXIT_FAILURE);
			}

      error=pthread_join(hilo, (void*)&respuesta);
			if(error != 0)
			{
				/*fprintf(stderr, "pthread_join: %s\n", perror(error));*/
        free(respuesta);
        free(es);
				exit(EXIT_FAILURE);
			}

			printf("El hilo %d ha esperado %d segundos, y ha devuelto %d. \n", i, es->tiempo, *respuesta);

    }

		free(respuesta);
		free(es);


		exit(EXIT_SUCCESS);

}
