#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>

#include "tcb_priority_queue.h"

/**
* @brief This struct contains the actual semaphore. Before attempting to aquire 
*   a semaphore, it must be initialised. 
*/
typedef struct s_OS_semaphore
{
    // Counts the current number of resources in the semaphore. A value equal 
    // to nResources means the sempahore has the maximum amount of resources
    // available. A value equal to 0 means the semaphore has no resources 
    // available for tasks to aquire. Each time a task aquires a resource, the 
    // counter decrements.
    volatile uint32_t  counter;
    
    // The maximum amount of resources tasks can aquire from the semaphore.
    size_t  nResources;
    
    // This field stores the priority queue for the tasks waiting for the 
    // semaphore. For a detailed explanation, see mutex.h.
    OS_tcbPriorityQueue_t  _waitingTasksQueue;
    
    // This field stores the waiting tasks in the task queue.
    OS_TCB_t*              _waitingTasks[MAX_TASKS];
} OS_sem_t;

/** 
* @brief Initialise a semaphore. This function must be called before the aquire
*   and release functions. 
* @param sem Pointer to the semaphore to initialise.
* @param nResources The number of resources available to aquire. For example, if 
*   there are nResources is set to 6, tasks can aquire the semaphore 6 times
*   before having to wait until a different task releases the semaphore.
*/ 
void OS_InitSemaphore(OS_sem_t* const sem, const size_t nResources);

/**
*  @brief Attempt to aquire a resource from the semaphore. If there are no 
*    resources available, the task that called this function will be put into
*    the wait state.
* @param sem Pointer to the semaphore to aquire.
*/
void OS_SemaphoreAquire(OS_sem_t* const sem);

/**
* @brief Release a resource back to the semaphore. If a task attempts to release
*   too many resources back to the semaphore, this function will simply return
*   without relasing any extra.
* @param sem Pointer to the semaphore to release resources back to.
*/
void OS_SemaphoreRelease(OS_sem_t* const sem);

/**
* @brief Get the value of a semaphore's counter field. This should be preferred
*   to directly accessing the counter field as this function guarentees atomic
*   access to counter field.
* @param sem The semaphore to get the counter field from.
* @return The value of the counter field.
*/
uint32_t OS_SemaphoreGetCount(OS_sem_t* const sem);

/**
* @brief Get the number of a semaphore's counter field.
* @param sem The semaphore to get the nResources field from.
* @return The value of the nResources field.
*/
uint32_t OS_SemaphoreGetNResources(OS_sem_t* const sem);

/**
* @brief Determine whether a semaphore is full and has no resources left
*   available.
* @param sem The semaphore in question.
* @return 1 if the semaphore is full, 0 if it is not and there are resources
*   available.
*/
uint32_t OS_SemaphoreFull(OS_sem_t* const sem);

/**
* @brief Determine whether a semaphore is empty, i.e., it has the maximum number 
*   of resources available.
* @param sem The semaphore in question.
* @return 1 if the semaphore is empty, 0 if it not.
*/
uint32_t OS_SemaphoreEmpty(OS_sem_t* const sem);

/**
* @brief Put the current task into a semaphore's waiting tasks queue. This 
*   function exists to allow a mechanism to put the current task to sleep if, 
*   for example, OS_SemaphoreFull() returns 1. An example use of this is in 
*   itc_queue.c.
* @param sem The semaphore whose waiting tasks list the current tcb will be 
*   moved into.
* @param checkCode Checkcode to remove race conditions. Can be retrieved using
*   OS_GetCheckCode().
*/
void OS_SemaphoreWait(OS_sem_t* const sem, const uint32_t checkCode);


/**
* @brief Notify any tasks waiting for a semaphore. Like OS_SemaphoreWait(), this
*   functions exists to allow mechanisms to notify tasks if for example, they 
*   were put to sleep when the semaphore is full.
* @param sem The semaphore whose waiting tasks will be notified.
*/
void OS_SemaphoreNotify(OS_sem_t* const sem);

#endif  // SEMAPHORE_H
