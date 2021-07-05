/*
** Fichero: ejercicio_prottemp.c
** Autor: Claudia Gonzalez Arteaga y Laura Iniesta Rodriguez
** Correo: claudia.gonzaleza@estudiante.uam.es y laura.iniesta@estudiante.uam.es
** Grupo: 2261
** Fecha: 21-03-2020
** Descripcion: programa ejercicio.c que cumple los requisitos indicados
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv){
  pid_t pid;
  int sign;

  if(argc != 3){
    printf("Introduce una señal y un pid \n");
    return -1;
  }

  sign=atoi(argv[2]);
  pid=-(atoi(argv[1]));


  if((kill(sign, pid)) != 0){
    perror("Error en la llamada de funcion");
    return (EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);

}
