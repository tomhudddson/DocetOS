#ifndef TCB_PRIORITY_QUEUE_H
#define TCB_PRIORITY_QUEUE_H

#include "task.h"

#define TCBPQ_ORDER_BY_PRIORITY 1
#define TCBPQ_ORDER_BY_DATA     2

/**
* @brief This structure contains all necessary data for the implementation
* of a priority queue for task-control blocks. Before using the priority 
* queue, it MUST be initialised otherwise a hard fault will occur, which can 
* be done using OS_InitTCBPriorityQueue().
*/
typedef struct s_TCBPriorityQueue 
{
    // This field is a pointer to an array of the task-control block pointers
    // used in the priority queue. This is the array the scheduler will 
    // use to schedule tasks, so it is imperitive that no other tasks intefere
    // with this array.
	OS_TCB_t** store;
    
    // This field holds the index at which there is a free space in the store
    // array for the next task to be added. 
	size_t length;
    
    // This field is the maximum number of tasks the store array can hold. 
    size_t nMaxTasks;
    
    // This field determines what the priority-queue will be ordered by. Set to
    // TCBPQ_ORDER_BY_PRIORITY to ensure the highest priority task will be at 
    // the front. Set to TCBPQ_ORDER_BY_DATA to ensure the task with the lowest
    // value of the data field will be at the front.
    uint32_t orderBy;
} OS_tcbPriorityQueue_t;

/**
* @brief Initialse the priority queue. 
* @param queue Pointer to the priority queue to initialise.
* @param store Pointer to an array of OS_TCB_t pointers. 
* @param nMaxTasks The maximum number of tasks the priority queue may hold. 
*   Note this should not be a zero-based number, instead it should be the actual
*   number of tasks.
* @param orderBy The field in the tcb struct in which to order the queue by. Set
*   to TCBPQ_ORDER_BY_PRIORITY to ensure the task with the highest priority is 
*   at the front, and set to TCBPQ_ORDER_BY_DATA to ensure the task with the 
*   lowest value in the data field is at the front.
*/
void OS_InitTCBPriorityQueue(OS_tcbPriorityQueue_t* const queue, 
                               OS_TCB_t** store,
                               const size_t nMaxTasks,
                               const uint32_t orderBy);

/**
* @brief Insert a tcb ino the priority queue. It will automatically get sorted.
* @param queue Pointer to the priority queue to insert a tcb into.
* @param tcb Pointer to the tcb to insert into the priority queue.
*/
void OS_TCBPriorityQueueInsert(OS_tcbPriorityQueue_t* const queue, 
                                 OS_TCB_t* const tcb);

/**
* @brief This function allows the caller to simultaneously retrieve the task at
*   the front of the queue whilst also removing it from the queue. Once removed,
*   the queue will re-sort itself.
* @param queue Pointer to the priority queue to extract the task from.
* @return Pointer to the task that was at the front of the queue.
* @return 0 if the queue is empty.
*/
OS_TCB_t* OS_TCBPriorityQueueExtract(OS_tcbPriorityQueue_t* const queue);

/** 
* @brief This function allows the caller to retrieve the task from the front of 
*   the queue without removing it from the queue. 
* @param queue Pointer to the priority queue to retrieve the task from.
* @return Pointer to highest priority tcb. If the queue is empty, 0 will be 
*   returned instead.
* @return 0 if the queue is empty.
*/
OS_TCB_t* OS_TCBPriorityQueuePeek(OS_tcbPriorityQueue_t* const queue);

/**
* @brief This functions removes a task from the queue and then re-orders the 
*   queue to maintain its order. If no task is found, no changes to the queue
*   will be made.
* @param queue The queue to remove the task from.
* @param tcb The task to remove from the queue.
*/ 
void OS_TCBPriorityQueueRemove(OS_tcbPriorityQueue_t* const queue, 
                                 const OS_TCB_t* const tcb); 

/**
* @brief This function re-sorts the queue if, for any reason, the queue is 
*   believed to be out of order.
* @param queue The queue to re-resort.
*/
void OS_TCB_PriorityQueueReSort(OS_tcbPriorityQueue_t* const queue);

/**
* @brief This function determines whether the priority queue is currently full.
* @return 1 if the queue is empty, 0 if it is not.
*/
uint32_t OS_TCBPriorityQueueFull(OS_tcbPriorityQueue_t* const queue);

/**
* @brief This function determines whether the priority queue is currently empty.
* @param Pointer to the priority queue in question.
* @return 1 if the queue is empty, 0 if it is not.
*/
uint32_t OS_TCBPriorityQueueEmpty(OS_tcbPriorityQueue_t* const queue);

/**
* @brief This function prints the priority queue to the terminal.
* @param queue The priority queue to print.
*/ 
void OS_TCBPriorityQueuePrint(const OS_tcbPriorityQueue_t* const queue);

#endif  // TCB_PRIORITY_QUEUE_H
