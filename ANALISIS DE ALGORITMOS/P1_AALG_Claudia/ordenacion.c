/**
 *
 * Descripcion: Implementacion de funciones de ordenacion
 *
 * Fichero: ordenacion.c
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 16-09-2019
 *
 */


#include "ordenacion.h"
#include <stdlib.h>
#include <stdio.h>

/***************************************************/
/* Funcion: InsertSort Fecha:28/09/2019            */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Rutina que ordena los números de                */
/* una tabla de menor a mayor                      */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* Salida:                                         */
/* int: numero de OB ejecutadas                    */
/***************************************************/
int InsertSort(int* tabla, int ip, int iu)
{
  int i=0, j = 0, ob=0;
  int aux;

  if(!tabla || ip<0 || iu<0) return ERR;

  for(i=ip+1;i<=iu;i++){

    aux = tabla[i];
    j=i-1;

    while(j >= ip && tabla[j] > aux){
      tabla[j+1] = tabla[j];
      j--;
      ob+=1;
    }

    if(j>=ip){
      ob++;
    }

    tabla[j+1] = aux;

  }

return ob;

}

/***************************************************/
/* Funcion: InsertSortInv Fecha:28/09/2019         */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Rutina que ordena los números de                */
/* una tabla de mayor a menor                      */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* Salida:                                         */
/* int: número de OB ejecutadas                    */
/***************************************************/
int InsertSortInv(int* tabla, int ip, int iu)
{
  int i=0, aux = 0, j = 0, ob=0;

  if(!tabla || ip<0 || iu<0) return ERR;

  for(i=ip+1;i<=iu;i++){

    aux = tabla[i];
    j=i-1;

    while(j >= ip && tabla[j] < aux){
      tabla[j+1] = tabla[j];
      j--;
      ob+=1;
    }
    if(j>=ip){
      ob++;
    }
    
    tabla[j+1] = aux;

  }
return ob;
}
