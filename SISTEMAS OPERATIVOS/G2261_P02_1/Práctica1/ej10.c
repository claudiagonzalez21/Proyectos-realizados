#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 32
#define MAX_CHAR 256
#define SHELL "(shell)$ "

int main(void) {
    char string[MAX_CHAR], *tok=NULL;
    char *argumentos[MAX_ARGS + 2];
    int i=0, estado;
    pid_t pid;
    int flag = 0;


    fprintf(stdout, SHELL);
    fflush(stdout);

    while(fgets(string, MAX_CHAR, stdin)){
      for (i = 0; i < strlen(string); i++) {
        flag = 0;
        if (string[i] == ' ') {
          flag = 1;
          break;
        }
      }

      i = 0;

      if (flag == 0) {
        argumentos[0] = strtok(string, "\n");
      }


      else {
        argumentos[0] = strtok(string, " ");
        //cogemos la primera palabra de string y la pongo en argumentos[0]
      }

      while((tok = strtok(NULL, "\n")) && i < MAX_ARGS){
        i++;
        argumentos[i] = tok;
      }
      argumentos[i + 1] = (char *)NULL;

      if((pid=fork())){
        if(pid <  0)
        {
          perror("fork");
          exit(EXIT_FAILURE);
        }
        wait(&estado);

        if(WIFEXITED(estado)){
          fprintf(stdout, "Estado: %d\n", WEXITSTATUS(estado));

        }
        if(WIFSIGNALED(estado)){
          fprintf(stdout, "Estado: %d\n", WTERMSIG(estado));

        }

        fprintf(stdout, SHELL);
        fflush(stdout);
      }

      else {
        if(execvp(argumentos[0], argumentos) == -1){
          perror("execvp");
          exit(EXIT_FAILURE);
        }
      }
    }

   fprintf(stdout, "[CTRL+D]\n");
   exit(EXIT_SUCCESS);
}
