/**
 *
 * Descripcion: Implementacion funciones para busqueda
 *
 * Fichero: busqueda.c
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 11-11-2016
 *
 */

#include "busqueda.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 *  Funciones de geracion de claves
 *
 *  Descripcion: Recibe el numero de claves que hay que generar
 *               en el parametro n_claves. Las claves generadas
 *               iran de 1 a max. Las claves se devuelven en
 *               el parametro claves que se debe reservar externamente
 *               a la funcion.
 */

/**
 *  Funcion: generador_claves_uniforme
 *               Esta fucnion genera todas las claves de 1 a max de forma
 *               secuencial. Si n_claves==max entonces se generan cada clave
 *               una unica vez.
 */
void generador_claves_uniforme(int *claves, int n_claves, int max){
  int i;

  for(i = 0; i < n_claves; i++) claves[i] = (i % max);

  return;
}

/**
 *  Funcion: generador_claves_potencial
 *               Esta funcion genera siguiendo una distribucion aproximadamente
 *               potencial. Siendo los valores mas pequenos mucho mas probables
 *               que los mas grandes. El valor 1 tiene una probabilidad del 50%,
 *               el dos del 17%, el tres el 9%, etc.
 */
void generador_claves_potencial(int *claves, int n_claves, int max){
  int i;

  for(i = 0; i < n_claves; i++) {
    claves[i] = .5+max/(max*((double)rand()/(RAND_MAX)));
  }

  return;
}

/*******************************************************/
/* Funcion: ini_diccionario Fecha: 28-10-2019          */
/* Autores: Claudia González y Laura Iniesta           */
/*                                                     */
/* Rutina que inicializa la estructuda PDICC y         */
/* reserva memoria                                     */
/*                                                     */
/* Entrada:                                            */
/* int tamanio: Tamanio de la tabla de la estructura   */
/* char orden: Indica el tipo de diccionario que se    */
/* va a crear, ordenado (1) o no ordenado (0)          */
/* Salida:                                             */
/* PDICC con éxito / NULL con fallo                    */
/*******************************************************/
PDICC ini_diccionario (int tamanio, char orden){
  PDICC dic = NULL;
  if(tamanio<0 || orden<0 || orden >1) return NULL;

  dic =(PDICC)malloc(sizeof(DICC));
  if(!dic) return NULL;

  dic->tamanio = tamanio;
  dic->n_datos = 0;
  dic->orden = orden;

  dic->tabla = (int*)malloc(tamanio*sizeof(int));
  if(!(dic->tabla)){
    free(dic);
    return NULL;
  }
  return dic;
}

/*******************************************************/
/* Funcion: libera_diccionario Fecha: 28-10-2019       */
/* Autores: Claudia González y Laura Iniesta           */
/*                                                     */
/* Rutina que libera la memoria reservada para un      */
/* TAD diccionario                                     */
/*                                                     */
/* Entrada:                                            */
/* PDICC pdicc: puntero a la estructura diccionario    */
/*******************************************************/
void libera_diccionario(PDICC pdicc){
  if(!pdicc){
    return;
  }
  pdicc->n_datos = 0;
	free(pdicc->tabla);
  free(pdicc);
}
/*******************************************************/
/* Funcion: inserta_diccionario Fecha: 28-10-2019      */
/* Autores: Claudia González y Laura Iniesta           */
/*                                                     */
/* Rutina que inserta una clave en el diccionario      */
/*                                                     */
/*                                                     */
/* Entrada:                                            */
/* PDICC pdicc: TAD diccionario                        */
/* int clave: Clave a insertar en el TAD               */
/* Salida:                                             */
/* OB con éxito / ERR con fallo                        */
/*******************************************************/
int inserta_diccionario(PDICC pdicc, int clave){
  int j= 0, ob=1;
	if(pdicc == NULL) return ERR;

    pdicc->tabla[pdicc->n_datos] = clave;

  if(pdicc->orden == ORDENADO){

        j=(pdicc->n_datos)-1;
        while (j >=0 && (pdicc->tabla[j])>clave){

                ob++;
                pdicc->tabla[j+1]= pdicc->tabla[j];
                j--;
        }
        pdicc->tabla[j+1]=clave;
        ob++;
  }

  pdicc->n_datos++;

  return ob;
}

/**********************************************************/
/* Funcion: inserción_masiva_diccionario Fecha: 28-10-2019*/
/* Autores: Claudia González y Laura Iniesta              */
/*                                                        */
/* Rutina que inserta varias claves en un diccionario     */
/* Mediante llamadas a inserta_diccionario                */
/*                                                        */
/* Entrada:                                               */
/* PDICC pdicc: TAD diccionario                           */
/* int* claves: Claves a insertar en el TAD               */
/* int n_claves: Número de claves a insertar en el TAD    */
/* Salida:                                                */
/* OB con éxito / ERR con fallo                           */
/**********************************************************/
int insercion_masiva_diccionario (PDICC pdicc,int *claves, int n_claves){
  int i=0, ob=0,flag=0;
	if(!claves || n_claves <0 || pdicc== NULL) return ERR;

    for(i=0;i<n_claves;i++){
      flag = inserta_diccionario(pdicc,claves[i]);
      if(flag == ERR) return ERR;
      ob+=flag;
    }
    return ob;
}

/**********************************************************/
/* Funcion: inserción_masiva_diccionario Fecha: 28-10-2019*/
/* Autores: Claudia González y Laura Iniesta              */
/*                                                        */
/* Rutina que busca una clave en el TAD usando la         */
/* función de búsqueda pasada como argumento              */
/*                                                        */
/* Entrada:                                               */
/* PDICC pdicc: TAD diccionario                           */
/* int clave: Clave a buscar en el TAD                    */
/* int *ppos: Puntero a la posición de la clave en el TAD */
/* pfunc_busqueda metodo: FUnción de búsqueda             */
/* Salida:                                                */
/* OB con éxito / ERR con fallo                           */
/**********************************************************/
int busca_diccionario(PDICC pdicc, int clave, int *ppos, pfunc_busqueda metodo){
  int p=0, u=0, flag = 0, ob=0;
  if(pdicc == NULL|| !ppos || !metodo) {

    return ERR;
  }

  u = (pdicc->n_datos)-1;

  if(pdicc->orden == NO_ORDENADO && metodo == bbin) {
    fprintf(stdout,"\nBbin sólo funciona con diccionarios Ordenados.\n");
    return ERR;
  }

  flag = metodo(pdicc->tabla,p,u,clave,ppos);

  if (flag == ERR) return ERR;

  ob = flag;

  return ob;
}


/* Funciones de busqueda del TAD Diccionario */

/***************************************************/
/* Funcion: bbin Fecha:28/11/2019            */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Rutina que busca una clave de una tabla de      */
/*elementos ordenada                               */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int P: posición del primer número               */
/* int U: posición del último número               */
/*int clave: clave a buscar                        */
/*int* ppos: posición de la clave encontrada       */
/* Salida:                                         */
/* int: numero de OB ejecutadas                    */
/***************************************************/
int bbin(int *tabla,int P,int U,int clave,int *ppos){
  int ob =0, m;

    if(!tabla || P<0 || U<0 || U<P || !ppos) return ERR;

    while(P<=U){
      m=(P+U)/2;
      if(tabla[m] == clave){
        ob++;
        (*ppos) = m;

        return ob;

      }else if(tabla[m] < clave){
        ob++;
        P=m+1;
      }else{
        ob++;
        U=m-1;
      }

    }

  *ppos = NO_ENCONTRADO;


  return ob;
}

/***************************************************/
/* Funcion: blin Fecha:28/11/2019            */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Rutina que busca una clave dentro de una lista. */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int P: posición del primer número               */
/* int U: posición del último número               */
/*int clave: clave a buscar                        */
/*int* ppos: posición de la clave encontrada       */
/* Salida:                                         */
/* int: numero de OB ejecutadas                    */
/***************************************************/
int blin(int *tabla,int P,int U,int clave,int *ppos){
  int ob =0,i,j;

  if(!tabla || P<0 || U<0 || U<P || !ppos) return ERR;
  i=P;
  j=U;
  while(i<=j){
    if(tabla[i] == clave){
      ob++;
      (*ppos)=i;
      return ob;
    }else{
      ob++;
      i++;
    }
  }


  (*ppos)=NO_ENCONTRADO;
  return ob;
}

/***************************************************/
/* Funcion: blin_auto Fecha:28/11/2019            */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Rutina que busca una clave dentro de una lista. */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int P: posición del primer número               */
/* int U: posición del último número               */
/*int clave: clave a buscar                        */
/*int* ppos: posición anterior de la clave encontrada*/
/* Salida:                                         */
/* int: numero de OB ejecutadas                    */
/***************************************************/
int blin_auto(int *tabla,int P,int U,int clave,int *ppos){
  int ob =0, aux = 0;
    if(!tabla || P<0 || U<0 || U<P || !ppos) return ERR;

    while(P<=U){
      if(tabla[P] == clave){
        ob++;
        if(P!=0){
          /*swap*/
          aux = tabla[P-1];
          tabla[P-1] = tabla[P];
          tabla[P] = aux;
          /*fin swap*/
          (*ppos)=P-1;
          return ob;
        }else{
          (*ppos)=P;
          return ob;
        }
      }else{
        ob++;
        P++;
      }
    }

    (*ppos)=NO_ENCONTRADO;

  return ob;
}
