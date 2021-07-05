/***********************************************/
/* Programa: ejercicio1     Fecha:             */
/* Autores:                                    */
/*                                             */
/* Programa que genera numeros aleatorios      */
/* entre dos numeros dados                     */
/*                                             */
/* Entrada: Linea de comandos                  */
/* -limInf: limite inferior                    */
/* -limSup: limite superior                    */
/* -numN: cantidad de numeros                  */
/* Salida: 0: OK, -1: ERR                      */
/***********************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "permutaciones.h"

/*Declaración de funciones privadas*/
int compare(const void *n, const void *m);

int main(int argc, char** argv)
{
  int i;
  int *aleat = NULL;
  unsigned int inf, sup, num, j;
  FILE *f;

  srand(time(NULL));

  if (argc != 7) {
    fprintf(stderr, "Error en los parametros de entrada:\n\n");
    fprintf(stderr, "%s -limInf <int> -limSup <int> -numN <int>\n", argv[0]);
    fprintf(stderr, "Donde:\n");
    fprintf(stderr, " -limInf : Limite inferior.\n");
    fprintf(stderr, " -limSup : Limite superior.\n");
    fprintf(stderr, " -numN : Cantidad de numeros a generar.\n");
    exit(-1);
  }
  printf("Practica numero 1, apartado 1\n");
  printf("Realizada por: Claudia González y Laura Iniesta\n");
  printf("Grupo: 1261\n");

  /* comprueba la linea de comandos */
  for(i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-limInf") == 0) {
      inf = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-limSup") == 0) {
      sup = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-numN") == 0) {
      num = atoi(argv[++i]);
    } else {
      fprintf(stderr, "Parametro %s es incorrecto\n", argv[i]);
    }
  }
  aleat = (int*) malloc (num * sizeof(int));

  if(!aleat){
    free(aleat);
    return -1;
  }

  f = fopen("histograma.txt", "w");

  /* imprimimos los datos */
  for(j = 0; j < num; j++) {
    aleat[j] = aleat_num(inf, sup);
    printf("%d\n",aleat[j]);
  }
/*Ordenamos los datos*/
 qsort(aleat, num, sizeof(int), &compare);

  /* imprimimos los datos ordenados en el .txt */
   for(j = 0; j < num; j++) {
     fprintf(f, "%d\n",aleat[j]);
   }
   free(aleat);

   fclose(f);
  return 0;
}


/***************************************************/
/* Funcion: compare Fecha:27-09-2019               */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Funcion que compara dos elementos               */
/*                                                 */
/* Entrada:                                        */
/* const void *_a: Primer número                   */
/* const void *_b: Segundo número                  */
/* Salida:                                         */
/* int: resultado de la resta de los dos números   */
/***************************************************/

int compare(const void *n, const void *m) {
        int *a, *b;

        a = (int *) n;
        b = (int *) m;

        return (*a - *b);
}
