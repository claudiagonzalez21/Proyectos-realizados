/**************************************************/
/* Programa: ejercicio4       Fecha:              */
/* Autores:                                       */
/*                                                */
/* Programa que Comprueba SelectSort              */
/*                                                */
/* Entrada: Linea de comandos                     */
/* -tamanio: numero elementos permutacion         */
/* Salida: 0: OK, -1: ERR                         */
/**************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "permutaciones.h"
#include "ordenacion.h"

int main(int argc, char** argv)
{
  int tamano, i, j, ret;
int* perm = NULL;
  /*int perm[10] ;*/

  srand(time(NULL));

  if (argc != 3) {
    fprintf(stderr, "Error en los parametros de entrada:\n\n");
    fprintf(stderr, "%s -tamanio <int>\n", argv[0]);
    fprintf(stderr, "Donde:\n");
    fprintf(stderr, " -tamanio : numero elementos permutacion.\n");
    return 0;
  }
  printf("Practica numero 1, apartado 4\n");
  printf("Realizada por: Claudia González y Laura Iniesta\n");
  printf("Grupo: 1262\n");

  /* comprueba la linea de comandos */
  for(i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-tamanio") == 0) {
      tamano = atoi(argv[++i]);
    } else {
      fprintf(stderr, "Parametro %s es incorrecto\n", argv[i]);
    }
  }

  perm = genera_perm(tamano);
/*BUCLE PERM ORDENADA*/
  /*for(j = 0; j < tamano; j++) {
    perm[j]=j;
  }*/

/*BUCLE PERM DESORDENADA*/
  /*for(j = 0, i = tamano; j < tamano; j++, i--) {
    perm[j]=j;
  }*/

  if (perm == NULL) { /* error */
    printf("Error: No hay memoria\n");
    exit(-1);
  }

  ret = quicksort(perm, 0, tamano-1);

  if (ret == ERR) {
    printf("Error: Error en BubbleSort\n");

    exit(-1);
  }

  for(j = 0; j < tamano; j++) {
    printf("%d \t", perm[j]);
  }

  printf("%d \t", ret);

  free(perm);

  return 0;
}
