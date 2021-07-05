/**
 *
 * Descripcion: Implementacion de funciones de tiempo
 *
 * Fichero: tiempos.c
 * Autor: Carlos Aguirre Maeso
 * Version: 1.0
 * Fecha: 16-09-2017
 *
 */

#include "tiempos.h"
#include "ordenacion.h"
#include "permutaciones.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


/****************************************************/
/* Funcion: tiempo_medio_ordenacion Fecha: 03-10-2019*/
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que ordena permutaciones calculando       */
/* las OB y el tiempo que tarda en ordenarla        */
/*                                                  */
/* Entrada:                                         */
/* pfunc_ordena: Puntero a la función de ordenación */
/* int n_perms: Número de permutaciones             */
/* int N: Tamaño de las permutaciones               */
/* PTIEMPO ptiempo: Puntero a la estructura tiempo  */
/* Salida:                                          */
/* OK con éxito / ERR con fallo                     */
/****************************************************/
short tiempo_medio_ordenacion(pfunc_ordena metodo,int n_perms, int N, PTIEMPO ptiempo){

  int** perm = NULL;
  int i, j = 0, ob = 0;
  clock_t inicio, fin;

  if(!metodo || !ptiempo || n_perms < 0 || N < 0) return ERR;

  inicio = clock();

  perm = genera_permutaciones(n_perms, N);
      ptiempo->medio_ob =0;
      ptiempo->medio_ob = 0;
      ptiempo->min_ob = 0;
      ptiempo->max_ob = 0;
  if(!perm) return ERR;

  for(i=0; i< n_perms; i++){

      ob = metodo(perm[i],j,N-1);
      ptiempo->medio_ob += ob;

      if(i == 0){
        ptiempo->min_ob = ob;
        ptiempo->max_ob = ob;
      }

      if(ptiempo->min_ob > ob){
        ptiempo->min_ob = ob;
      }

      if(ptiempo->max_ob < ob){
        ptiempo->max_ob = ob;
      }

  }

  ptiempo->medio_ob /= n_perms;
  ptiempo->n_elems = n_perms;
  ptiempo->N = N;

  fin = clock();

  ptiempo->tiempo = (fin - inicio);
  ptiempo->tiempo/= (double)(n_perms);
  ptiempo->tiempo/= CLOCKS_PER_SEC;

  for (i=0;i<n_perms;i++){
      free(perm[i]);
    }

 free(perm);


  return OK;
}

/*******************************************************/
/* Funcion: genera_tiempos_ordenacion Fecha: 03-10-2019*/
/* Autores: Claudia González y Laura Iniesta           */
/*                                                     */
/* Rutina que llama a la función que genera y          */
/* ordena permutaciones y a la que guarda              */
/* la información sobre las OB y el tiempo de ejecución*/
/*                                                     */
/* Entrada:                                            */
/* pfunc_ordena: Puntero a la función de ordenación    */
/* char* fichero:Nombre del fichero a generar          */
/* int n_perms: Número de permutaciones                */
/* int num_min: Tamaño mínimo de las permutaciones     */
/* int num_max: Tamaño máximo de las permutaciones     */
/* int incr: Incremento de tamaño en cada permutacion  */
/* Salida:                                             */
/* OK con éxito / ERR con fallo                        */
/*******************************************************/
short genera_tiempos_ordenacion(pfunc_ordena metodo, char* fichero, int num_min, int num_max, int incr, int n_perms){

  int i = 0;
  PTIEMPO pt = NULL;
  int flag = OK;


  if(!fichero|| !metodo || num_min>num_max || incr < 0 || n_perms <=0 || num_min < 0 || num_max < 0 ) return ERR;

  pt = (PTIEMPO)malloc(sizeof(TIEMPO));

  if(!pt) return ERR;

  pt->N = 0;
  pt->n_elems = 0;
  pt->tiempo = 0;
  pt->medio_ob = 0;
  pt->min_ob = 0;
  pt->max_ob = 0;

  for(i = num_min; i <= num_max; i += incr){

      flag = tiempo_medio_ordenacion(metodo, n_perms, i, pt);
      if(flag == ERR) return ERR;

      flag = guarda_tabla_tiempos(fichero, pt, 1);
      if(flag == ERR) return ERR;
    }

    free(pt);

    return flag;
}

/****************************************************/
/* Funcion: guarda_tabla_tiempos Fecha: 3/10/19     */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que Imprime en un fichero la información  */
/* del puntero tiempo sobre las OB y los tiempos    */
/* de las permutaciones generadas y ordeenadas      */
/* Entrada:                                         */
/* char* fichero:Nombre del fichero a generar       */
/* PTIEMPO tiempo:Puntero a Tiempo, guarda la info. */
/* int n_tiempos: Número de punteros que se pasan   */
/* Salida:                                          */
/* OK con éxito / ERR con fallo                     */
/****************************************************/
short guarda_tabla_tiempos(char* fichero, PTIEMPO tiempo, int n_tiempos){
    FILE *f;
    if(!fichero || !tiempo || n_tiempos < 0) return ERR;

    f = fopen(fichero, "a");

    if(!f) return ERR;

    fprintf(f, "%d %d %d %f %.15f\n", tiempo->N, tiempo->min_ob, tiempo->max_ob, tiempo->medio_ob, tiempo->tiempo);

/*
    fprintf(f, "El número de elementos a promediar:\n");
    fprintf(f, "%d\n", tiempo->n_elems);

    fprintf(f, "El tamaño de los elementos a promediar:\n");
    fprintf(f, "%d\n", tiempo->N);

    fprintf(f, "El número mínimo de OB:\n");
    fprintf(f, "%d\n", tiempo->min_ob);

    fprintf(f, "El número máximo de OB:\n");
    fprintf(f, "%d\n", tiempo->max_ob);

    fprintf(f, "El número promedio de OB:\n");
    fprintf(f, "%f\n", tiempo->medio_ob);

    fprintf(f, "El tiempo promedio de reloj:\n");
    fprintf(f, "%f\n", tiempo->tiempo);
*/
    fclose(f);

    return OK;

}
