#ifndef _SORT_H
#define _SORT_H

#include <mqueue.h>
#include <semaphore.h>
#include <sys/types.h>
#include "global.h"

/* Constants. */
#define MAX_DATA 100000
#define MAX_LEVELS 10
#define MAX_PARTS 512
#define MAX_STRING 1024

#define PLOT_PERIOD 1
#define NO_MID -1

/* Type definitions. */

/* Completed flag for the tasks. */
typedef enum {
    INCOMPLETE,
    SENT,
    PROCESSING,
    COMPLETED
} Completed;

/* Task. */
typedef struct {
    Completed completed;
    int ini;
    int mid;
    int end;
} Task;

/* Structure for the sorting problem. */
typedef struct{
    Task tasks[MAX_LEVELS][MAX_PARTS];
    int data[MAX_DATA];
    int delay;
    int n_elements;
    int n_levels;
    int n_processes;
    pid_t ppid;
    sem_t sem;
} Sort;
/* Estado. */
typedef struct {
    pid_t proceso;
    int level;
    int parte;
    char* estado_task;
} Estado;

/* Prototypes. */

/**
 * Sorts an array using bubble-sort.
 * @method bubble_sort
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  vector      Array with the data.
 * @param  n_elements  Number of elements in the array.
 * @param  delay       Delay for the algorithm.
 * @return             ERROR in case of error, OK otherwise.
 */
Status bubble_sort(int *vector, int n_elements, int delay, sem_t sem);

/**
 * Merges two ordered parts of an array keeping the global order.
 * @method merge
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  vector     Array with the data.
 * @param  middle     Division between the first and second parts.
 * @param  n_elements Number of elements in the array.
 * @param  delay      Delay for the algorithm.
 * @return            ERROR in case of error, OK otherwise.
 */
Status merge(int *vector, int middle, int n_elements, int delay, sem_t sem);

/**
 * Computes the number of parts (division) for a certain level of the sorting
 * algorithm.
 * @method get_number_parts
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  level            Level of the algorithm.
 * @param  n_levels         Total number of levels in the algorithm.
 * @return                  Number of parts in the level.
 */
int get_number_parts(int level, int n_levels);

/**
 * Initializes the sort structure.
 * @method init_sort
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  file_name   File with the data.
 * @param  sort        Pointer to the sort structure.
 * @param  n_levels    Total number of levels in the algorithm.
 * @param  n_processes Number of processes.
 * @param  delay       Delay for the algorithm.
 * @return             ERROR in case of error, OK otherwise.
 */
Status init_sort(char *file_name, Sort *sort, int n_levels, int n_processes, int delay);

/**
 * Checks if a task is ready to be solved.
 * @method check_task_ready
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  sort             Pointer to the sort structure.
 * @param  level            Level of the algorithm.
 * @param  part             Part inside the level.
 * @return                  FALSE if it is not ready, TRUE otherwise.
 */
Bool check_task_ready(Sort *sort, int level, int part);

/**
 * Solves a single task of the sorting algorithm.
 * @method solve_task
 * @date   2020-04-09
 * @author Teaching team of SOPER
 * @param  sort       Pointer to the sort structure.
 * @param  level      Level of the algorithm.
 * @param  part       Part inside the level.
 * @return            ERROR in case of error, OK otherwise.
 */
Status solve_task(Sort *sort, int level, int part);

/**
 * Solves a sorting problem using a multiprocesses.
 * @method sort_single_process
 * @date   2020-04-09
 * @author Laura Iniesta y Claudia González
 * @param  file_name        File with the data.
 * @param  n_levels         Total number of levels in the algorithm.
 * @param  n_processes      Number of processes.
 * @param  delay            Delay for the algorithm.
 * @return                  ERROR in case of error, OK otherwise.
 */
Status sort_multi_process(char *file_name, int n_levels, int n_processes, int delay);

/**
 * Shows the current information of the workers
 * @method ilustrador
 * @date   2020-05
 * @author Claudia González y Laura Iniesta
 * @param  sort       Pointer to the sort structure.
 * @param  id         id of the process.
 * @param  trab_ilus  pipes the ilustrator reads from.
  * @param  ilus_trab  pipes the ilustrator writes in.
 * @return            ERROR in case of error, OK otherwise.
 */
int ilustrador(Sort *sort, pid_t id, int trab_ilus[MAX_PARTS][2], int ilus_trab[MAX_PARTS][2]);

/**
 * Solves a task received by a queue
 * @method trabajador
 * @date   2020-05
 * @author Claudia González y Laura Iniesta
 * @param  sort       Pointer to the sort structure.
 * @param  id         id of the process.
 * @param  trab_ilus  pipes the worker writes in.
  * @param  ilus_trab  pipes the workers reads form.
 * @return            ERROR in case of error, OK otherwise.
 */
int trabajador (Sort * sort, pid_t id,int trab_ilus[2], int ilus_trab[2]);

#endif
