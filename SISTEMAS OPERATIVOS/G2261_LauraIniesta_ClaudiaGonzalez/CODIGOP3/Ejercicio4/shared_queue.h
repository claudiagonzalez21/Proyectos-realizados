#ifndef SHARED_QUEUE_H
#define SHARED_QUEUE_H
#include <semaphore.h>
#include <signal.h>
#define QUEUE_BUFFER_MAX 10

/**
 * @brief Queue buffer in shared memory.
 */

typedef struct{
                size_t head; //!< Head of the queue.
                size_t count; //!< Element count.
                char buffer[QUEUE_BUFFER_MAX]; //!< Buffer where elements must be placed.
                pid_t id;
} QueueMemory;

/**
 * @brief Shared queue ADT.
 */

typedef struct{
                sem_t * mutex; //!< Mutex protecting the shared memory.
                sem_t * fill_count; //!< Semaphore with the number of elements filled in memory.
                sem_t * empty_count; //!< Semaphore with the number of empty spaces in memory.
                QueueMemory * memory; //!< Shared memory.
} SharedQueue;


/**
 * @brief Create the queue.
 *
 * @param [out] queue Queue.
 */

void SharedQueue_create(SharedQueue **queue, pid_t id);

/**
 * @brief Open the queue.
 *
 * @param [out] queue Queue.
 */

void SharedQueue_open(SharedQueue **queue);

/**
 * Add an element to the queue.
 *
 * @param [in] queue Queue.
 * @param [in] element Element to add.
 */

void SharedQueue_enqueue(SharedQueue *queue, int element);

/**
 * Remove an element from the queue.
 *
 * @param [in] queue Queue.
 *
 * @return Element removed.
 */

int SharedQueue_dequeue(SharedQueue *queue);

/**
 * @brief Close and delete the queue.
 *
 * @param [in] queue Queue to remove.
 */

void SharedQueue_delete(SharedQueue *queue);

/**
 * @brief Close the queue.
 *
 * @param [in] queue Queue to close.
 */

void SharedQueue_close(SharedQueue *queue);


#endif
