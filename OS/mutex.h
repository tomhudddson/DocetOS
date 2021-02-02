#ifndef MUTEX_H
#define MUTEX_H

#include "tcb_priority_queue.h"

/**
* @brief This struct contains the actual mutex. Before attempting to aquire or 
*   release a mutex, it must be initialised.
*/
typedef struct s_Mutex 
{
    // This stores the task that currently owns the mutex. If the mutex is not 
    // owned by any task, tcb will be equal to 0.
    OS_TCB_t*  tcb;
    
    volatile uint32_t  counter;
    
    // This field stores the priority queue in which waiting tasks are stored
    // in highest priority order. 
    // 
    // When a task realeases a mutex and notifies any waiting tasks, the highest
    // priority task gets moved from the waiting queue and into the running 
    // tasks queue. 
    // 
    // There is no possibility of a task waiting for two mutexes
    // simultaneously so the maximum number of waiting tasks is equal 
    // to MAX_TASKS. Therefore, it is possible to have a single static global 
    // waiting queue in mutex.c. However, having a global waiting tasks queue 
    // could mean that a task releasing a mutex and notifying the waiting tasks 
    // results in a task waiting for a different mutex being moved to the 
    // running tasks queue if that task has a higher priority than the task that
    // owns the original mutex. To prevent this, the waiting tasks queue and the
    // array that stores them is local to each mutex, even though it uses much 
    // more memory.
    OS_tcbPriorityQueue_t _waitingTaskQueue;
    
    // This field is used by the waiting task queue to store the waiting tasks. 
    OS_TCB_t*             _waitingTasks[MAX_TASKS];
} OS_mutex_t;

/**
* @brief Initialise a mutex. This function must be called before attempting to 
*   aquire or release a mutex.
* @param mutex Pointer to the mutex to initialise.
*/
void OS_InitMutex(OS_mutex_t* const mutex);

/**
* @brief Attempt to aquire a mutex. If the mutex is currently owned (has already 
*   been aquired by a different task), then the task calling this function will
*   be put into the wait state.
* @param mutex Pointer to the mutex to aquire.
*/
void OS_MutexAquire(OS_mutex_t* const mutex);

/**
* @brief Release a task's ownership of a mutex. If the mutex is not currently 
*   owned, or the task calling this function does not own the mutex, the 
*   function will simply return.
* @param mutex Pointer to the mutex to release.
*/
void OS_MutexRelease(OS_mutex_t* const mutex);

#endif  // MUTEX_H
