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
/***************************************************/
/* Funcion: mergesort Fecha:17/10/2019             */
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
/* OK con éxito / ERR con fallo                    */
/***************************************************/
int mergesort(int* tabla, int ip, int iu){
  int aux = 0, ob =0;

  if(!tabla || ip< 0 || iu<0 || ip>iu) return ERR;

  if (tabla[ip]==tabla[iu]){

      return OK;
    }

  else {
      aux = (ip+iu)/2;
      ob+=mergesort(tabla,ip,aux);
      ob+=mergesort(tabla,aux+1,iu);
    }

    ob+=merge(tabla,ip,iu,aux);

  return ob;
}
/***************************************************/
/* Funcion: merge         Fecha:17/10/2019         */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Rutina que ordena los números de                */
/* una tabla de menor a mayor                      */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* int imedio: posición de la mitad de la tabla    */
/* Salida:                                         */
/* OK con éxito / ERR con fallo                    */
/***************************************************/
int merge(int* tabla, int ip, int iu, int imedio){
  int* taux = NULL;
  int i=0 ,j=0, k=0, ob=0;
  int tamanio=iu+1;

  if(!tabla || ip< 0 || iu<0 || ip>iu) return ERR;

  taux = (int*)malloc(tamanio*sizeof(int));
  if(!taux)return ERR;


  i=ip;
  j=imedio+1;
  k=ip;

  while (i<=imedio && j<= iu){
    if(tabla[i]<tabla[j]){
      taux[k]=tabla[i];
      i++;

    }else{
      taux[k]=tabla[j];
      j++;
    }
    k++;
    ob++;

  }
  if(i>imedio){
    while(j<=iu){
      taux[k]=tabla[j];
      j++;
      k++;
    }
  }else if(j>iu){
    while(i<=imedio){
      taux[k]=tabla[i];
      i++;
      k++;
    }
  }



  for(k=ip;k<tamanio;k++){
    tabla[k] = taux[k];
  }

  free(taux);
  return ob;
}
/***************************************************/
/* Funcion:quicksort            Fecha:31/12/2019   */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Función que recorre recursivamente la tabla     */
/* llamando a partir para que la ordene            */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/*                                                 */
/* Salida:                                         */
/* OB con éxito / ERR con fallo                    */
/***************************************************/
int quicksort(int* tabla, int ip, int iu){
  int ob=0, m=0, pos=0;

  if(ip > iu || !tabla || ip < 0 || iu < 0){
    return ERR;
  }

  if(ip==iu){
    return ob;
  }else{
    ob+=partir(tabla, ip, iu, &pos);
    m=pos;


    if (ip < m-1){
      ob+= quicksort(tabla, ip, m-1);

    }

    if (m+1 < iu){
      ob+= quicksort(tabla, m+1, iu);
    }
  }

  return ob;
}
/***************************************************/
/* Funcion:quicksort_stat        Fecha:02/11/2019  */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Función que recorre recursivamente la tabla     */
/* llamando a partir_stat para que la ordene       */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/*                                                 */
/* Salida:                                         */
/* OB con éxito / ERR con fallo                    */
/***************************************************/
int quicksort_stat(int* tabla, int ip, int iu){
  int ob=0, m=0, pos=0;

  if(ip > iu || !tabla || ip < 0 || iu < 0){
    return ERR;
  }

  if(ip==iu){
    return ob;
  }


  else{
    ob+=partir_stat(tabla, ip, iu, &pos);
    m=pos;


    if (ip < m-1){
      ob = ob+quicksort_stat(tabla, ip, m-1);
    }

    if (m+1 < iu){
      ob = ob+quicksort_stat(tabla ,m+1, iu);
    }
  }

  return ob;
}
/***************************************************/
/* Funcion:quicksort_avg        Fecha:02/11/2019   */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Función que recorre recursivamente la tabla     */
/* llamando a partir_avg para que la ordene        */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/*                                                 */
/* Salida:                                         */
/* OB con éxito / ERR con fallo                    */
/***************************************************/
int quicksort_avg(int* tabla, int ip, int iu){
  int ob=0, m=0, pos=0;

  if(ip > iu || !tabla || ip < 0 || iu < 0){
    return ERR;
  }

  if(ip==iu){
    return ob;
  }


  else{
    ob+=partir_avg(tabla, ip, iu, &pos);
    m=pos;


    if (ip < m-1){
      ob = ob+quicksort_avg(tabla, ip, m-1);
    }

    if (m+1 < iu){
      ob = ob+quicksort_avg(tabla ,m+1, iu);
    }
  }

  return ob;
}
/***************************************************/
/* Funcion: partir            Fecha:31/10/2019     */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Función que calcula las OB y ordena la tabla    */
/* desde la posición obtenida en medio             */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* int *pos: Puntero a la posición del pivote      */
/* Salida:                                         */
/* Nº OB con éxito / ERR con fallo                 */
/***************************************************/
int partir(int* tabla, int ip, int iu,int *pos){
  int aux=0, ob=0, i,m=0;

  if(ip > iu || !tabla || ip < 0 || iu < 0 || !pos){
    return ERR;
  }

  ob=medio(tabla, ip, iu, pos);
  if(ob == ERR){
    return ERR;
  }



  aux=tabla[ip];
  tabla[ip]=tabla[*pos];
  tabla[*pos]=aux;

  (*pos)=ip;
  m=*pos;

  for(i=ip+1; i<=iu; i++){
    ob++;
    if (tabla[i]<tabla[m]){
      (*pos)++;
      aux=tabla[i];
      tabla[i]=tabla[*pos];
      tabla[*pos]=aux;
    }

  }

  aux=tabla[ip];
  tabla[ip]=tabla[*pos];
  tabla[*pos]=aux;

  return ob;
}
/***************************************************/
/* Funcion: partir_avg        Fecha:02/11/2019     */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Función que calcula las OB y ordena la tabla    */
/* desde la posición obtenida en medio_avg         */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* int *pos: Puntero a la posición del pivote      */
/* Salida:                                         */
/* Nº OB con éxito / ERR con fallo                 */
/***************************************************/
int partir_avg(int* tabla, int ip, int iu,int *pos){
  int aux=0, ob=0, i,m=0;

  if(ip > iu || !tabla || ip < 0 || iu < 0 || !pos){
    return ERR;
  }

  ob=medio_avg(tabla, ip, iu, pos);
  if(ob == ERR){
    return ERR;
  }



  aux=tabla[ip];
  tabla[ip]=tabla[*pos];
  tabla[*pos]=aux;

  (*pos)=ip;
  m=*pos;

  for(i=ip+1; i<=iu; i++){
    ob++;
    if (tabla[i]<tabla[m]){
      (*pos)++;
      aux=tabla[i];
      tabla[i]=tabla[*pos];
      tabla[*pos]=aux;
    }

  }

  aux=tabla[ip];
  tabla[ip]=tabla[*pos];
  tabla[*pos]=aux;

  return ob;
}
/***************************************************/
/* Funcion: partir_stat       Fecha:02/11/2019     */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/* Función que calcula las OB y ordena la tabla    */
/* desde la posición obtenida en medio_stat        */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* int *pos: Puntero a la posición del pivote      */
/* Salida:                                         */
/* Nº OB con éxito / ERR con fallo                 */
/***************************************************/
int partir_stat(int* tabla, int ip, int iu,int *pos){
  int aux=0, ob=0, i,m=0;

  if(ip > iu || !tabla || ip < 0 || iu < 0 || !pos){
    return ERR;
  }

  ob=medio_stat(tabla, ip, iu, pos);
  if(ob == ERR){
    return ERR;
  }



  aux=tabla[ip];
  tabla[ip]=tabla[*pos];
  tabla[*pos]=aux;

  (*pos)=ip;
  m=*pos;

  for(i=ip+1; i<=iu; i++){
    ob++;
    if (tabla[i]<tabla[m]){
      (*pos)++;
      aux=tabla[i];
      tabla[i]=tabla[*pos];
      tabla[*pos]=aux;
    }

  }

  aux=tabla[ip];
  tabla[ip]=tabla[*pos];
  tabla[*pos]=aux;

  return ob;
}
/***************************************************/
/* Funcion: medio_stat    Fecha:2/11/2019          */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/*Esta función compara los elemepntos en las       */
/* posiciones de la tabla ip, iu y la mitad        */
/* y el que tenga un valor intermedio              */
/* pasa a ser el pivote                            */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* int *pos: Puntero a la posición del pivote      */
/* Salida:                                         */
/* Nº OB con éxito / ERR con fallo                 */
/***************************************************/
int medio_stat(int *tabla, int ip, int iu,int *pos){
  int m=0, ob=0;
  m=(iu+ip)/2;

  if(ip > iu || !tabla || ip < 0 || iu < 0 || !pos){
    return ERR;
  }
  ob=3;
  if(tabla[ip] < tabla[m]){
    if(tabla[m] < tabla[iu]){
      *pos=m;
      return ob;
    }else if(tabla[ip] < tabla[iu]){
      *pos=iu;
      return ob;
    }else{
      *pos=ip;
      return ob;
    }
  }else{
    if(tabla[ip] < tabla[iu]){
      *pos=ip;
      return ob;
    }else if(tabla[m] < tabla[iu]){
      *pos=m;
      return ob;
    }else{
      *pos=iu;
      return ob;
    }
  }


  return ob;
}
/***************************************************/
/* Funcion: medio    Fecha:2/11/2019          */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/*Esta función coloca el pivote de la tabla  en    */
/* la posición inicial de la tabla                 */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* int *pos: Puntero a la posición del pivote      */
/* Salida:                                         */
/* Nº OB con éxito / ERR con fallo                 */
/***************************************************/
int medio(int *tabla, int ip, int iu,int *pos){

  if(ip > iu || !tabla || ip < 0 || iu < 0 || !pos){
    return ERR;
  }

  *pos=ip;

   return 0;
}
/***************************************************/
/* Funcion: medio_avg     Fecha:2/11/2019          */
/* Autores: Claudia González y Laura Iniesta       */
/*                                                 */
/*Esta función coloca el pivote de la tabla  en    */
/* la posición media de la tabla                   */
/*                                                 */
/* Entrada:                                        */
/* int* tabla: Tabla a ordenar                     */
/* int ip: posición del primer número              */
/* int iu: posición del último número              */
/* int *pos: Puntero a la posición del pivote      */
/* Salida:                                         */
/* Nº OB con éxito / ERR con fallo                 */
/***************************************************/

int medio_avg(int *tabla, int ip, int iu, int *pos){

  if(ip > iu || !tabla || ip < 0 || iu < 0 || !pos){
    return ERR;
  }

  *pos=(ip+ iu)/2;

  return 0;
}
