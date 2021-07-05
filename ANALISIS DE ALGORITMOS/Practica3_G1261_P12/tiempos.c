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
#include "busqueda.h"
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

    fclose(f);

    return OK;

}



/*******************************************************/
/* Funcion: genera_tiempos_busqueda Fecha: 01-12-2019*/
/* Autores: Claudia González y Laura Iniesta           */
/*                                                     */
/* Rutina que llama a la función que genera y          */
/* ordena permutaciones y a la que guarda              */
/* la información sobre las OB y el tiempo de ejecución*/
/*                                                     */
/* Entrada:                                            */
/* pfunc_busqueda: Puntero a la función de busqueda    */
/* pfunc_generador_claves: Puntero a la funcion generador*/
/* int orden: Indica si el diccionario esta ordenadao o no*/
/* char* fichero:Nombre del fichero a generar          */
/* int num_min: Tamaño mínimo de las permutaciones     */
/* int num_max: Tamaño máximo de las permutaciones     */
/* int incr: Incremento de tamaño en cada permutacion  */
/* int n_perms: Número de permutaciones                */
/* Salida:                                             */
/* OK con éxito / ERR con fallo                        */
/*******************************************************/
short genera_tiempos_busqueda(pfunc_busqueda metodo, pfunc_generador_claves generador, int orden, char* fichero,int num_min, int num_max,int incr, int n_veces){

  PTIEMPO pt = NULL;
  int i = 0;
  int flag = OK;

  if(!metodo|| !generador || orden< 0 || orden >1 || !fichero || num_min>num_max || num_max < 0 || num_min <0 || incr <0 || n_veces <0){
    return ERR;
  }

  pt = (PTIEMPO)malloc(sizeof(TIEMPO));

  if(!pt) return ERR;

  pt->N = 0;
  pt->n_elems = 0;
  pt->tiempo = 0;
  pt->medio_ob = 0;
  pt->min_ob = 0;
  pt->max_ob = 0;

  for(i = num_min; i < num_max; i += incr){

      flag = tiempo_medio_busqueda(metodo, generador,orden, i,n_veces, pt);

      if(flag == ERR) return ERR;

      flag = guarda_tabla_tiempos(fichero, pt, 1);

      if(flag == ERR) return ERR;
    }

    free(pt);

    return flag;

}
/****************************************************/
/* Funcion: tiempo_medio_busqueda Fecha: 1-12-2019  */
/* Autores: Claudia González y Laura Iniesta        */
/*                                                  */
/* Rutina que ordena permutaciones calculando       */
/* las OB y el tiempo que tarda en ordenarla        */
/*                                                  */
/* Entrada:                                         */
/* pfunc_busqueda: Puntero a la función de busqueda */
/* pfunc_generador_claves: Puntero a la funcion generador */
/* int orden: Indica si el diccionario esta ordenadao o no */
/* int n_veces: Número de permutaciones             */
/* int N: Tamaño de las permutaciones               */
/* PTIEMPO ptiempo: Puntero a la estructura tiempo  */
/* Salida:                                          */
/* OK con éxito / ERR con fallo                     */
/****************************************************/
short tiempo_medio_busqueda(pfunc_busqueda metodo, pfunc_generador_claves generador,int orden,int N,int n_veces,PTIEMPO ptiempo){
  PDICC pdicc=NULL;
  int *perm = NULL;
  int flag =0, ob =0, i;
  int pos, n;
  int* tabla = NULL;
  clock_t inicio, fin;

  if(!metodo || !generador || orden < 0 || orden > 1 || n_veces<=0 || !ptiempo) return ERR;

    n = N*n_veces;

    ptiempo->N = 0;
    ptiempo->n_elems = 0;
    ptiempo->tiempo = 0;
    ptiempo->medio_ob = 0;
    ptiempo->min_ob = 0;
    ptiempo->max_ob = 0;

    tabla = (int*)malloc(sizeof(int)*n);
    if(!tabla){
      return ERR;
    }

    perm = genera_perm(N);
    if(!perm) {
      free(tabla);
      return ERR;
    }

    pdicc = ini_diccionario(N, orden);
    if(!pdicc){
      free(tabla);
      free(perm);
      return ERR;
    }

    flag = insercion_masiva_diccionario(pdicc,perm,N);
    if(flag == ERR){
      libera_diccionario(pdicc);
      free(perm);
      free(tabla);
      return ERR;
    }

    generador(tabla, n, N);

    inicio = clock();



    for(i=0; i< n; i++){

          ob = busca_diccionario(pdicc, tabla[i], &pos, metodo);
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


      fin = clock();

    ptiempo->tiempo = (fin - inicio);
    ptiempo->medio_ob /= n;
    ptiempo->n_elems = n;
    ptiempo->N = N;
    ptiempo->tiempo/= (double)(n);
    ptiempo->tiempo/= CLOCKS_PER_SEC;

   free(tabla);
   free(perm);
   libera_diccionario(pdicc);

  return OK;

}
