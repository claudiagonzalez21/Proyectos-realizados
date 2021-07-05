/**
 *
 * Descripcion: Implementacion de funciones de generacion de permutaciones
 *
 * Fichero: permutaciones.c
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 16-09-2017
 *
 */


#include "permutaciones.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
/***************************************************/
/* Funcion: aleat_num Fecha:19/09/2019             */
/* Autores: Claudia González y Laura Iniesta      */
/*                                                 */
/* Rutina que genera un numero aleatorio           */
/* entre dos numeros dados                         */
/*                                                 */
/* Entrada:                                        */
/* int inf: limite inferior                        */
/* int sup: limite superior                        */
/* Salida:                                         */
/* int: numero aleatorio                           */
/***************************************************/
int aleat_num(int inf, int sup)
{
  int num = 0;

  if( inf < 0 || sup < 0 || sup<inf) return ERR;

  num = inf+(int)(((sup-inf+1.0)*rand())/(RAND_MAX+1.0));

  return num;
}

/***************************************************/
/* Funcion: genera_perm Fecha:26-09-2019           */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Rutina que genera una permutacion               */
/* aleatoria                                       */
/*                                                 */
/* Entrada:                                        */
/* int n: Numero de elementos de la                */
/* permutacion                                     */
/* Salida:                                         */
/* int *: puntero a un array de enteros            */
/* que contiene a la permutacion                   */
/* o NULL en caso de error                         */
/***************************************************/
int* genera_perm(int N)
{
  int i, aux = 0, aux2 = -1;
  int *perm = NULL;

  if( N < 0) return NULL;
  /*Comprobamos que el tamaño de la permutacion no sea negativo
  pero al probar el ej2 y meter un num de permutaciones negativo
  falla porque no se hace comprobacion en el ejercicio2.c*/

    perm = (int*)malloc(N* sizeof (int));

    if(perm == NULL){
      free(perm);
      return NULL;
    }

    for(i = 0; i < N; i++){
      perm[i] = i;
    }

    for(i = 0; i < N; i++){
      aux = aleat_num(i, N-1);
        aux2 = perm[i] ;
        perm[i] = perm[aux];
        perm[aux] = aux2;
    }



  return perm;
}

/***************************************************/
/* Funcion: genera_permutaciones Fecha:29-09-2019  */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Funcion que genera n_perms permutaciones        */
/* aleatorias de tamanio elementos                 */
/*                                                 */
/* Entrada:                                        */
/* int n_perms: Numero de permutaciones            */
/* int N: Numero de elementos de cada              */
/* permutacion                                     */
/* Salida:                                         */
/* int**: Array de punteros a enteros              */
/* que apuntan a cada una de las                   */
/* permutaciones                                   */
/* NULL en caso de error                           */
/***************************************************/
int** genera_permutaciones(int n_perms, int N)
{
  int **perm = NULL;
  int i;

  if( n_perms < 0 || N < 0) return NULL;

  perm = (int**)malloc(n_perms * sizeof(int*));
      if(!perm) return NULL;

  if(perm == NULL){

    for(i=0; i< n_perms; i++){
      free(perm[i]);
    }
     free(perm);

     return NULL;
   }

  for(i=0;i < n_perms;i++){

  perm[i] = genera_perm(N);

  }

  return perm;

}
