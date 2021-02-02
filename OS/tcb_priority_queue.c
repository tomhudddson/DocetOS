#include "tcb_priority_queue.h"

#include <stdio.h>

/* 
The priority queue is implemented using a minimum heap. This means, the lowest
priority value, or the lowest data value will always be at the front of the 
heap's store array, i.e. the highest priority task will always be at element 0 
if the heap's store array. Note, the priority value is different to the actual
priority - the value is the integer the priority level is defined at. The lower
the value, the higher the priority.

When a tcb is added using OS_TCBPriorityQueueInsert
*/

/* This function returns the value of the data that the queue is being ordered
by at a specific index. */
static uint32_t ElementValue(const OS_tcbPriorityQueue_t* const queue, 
                               const uint32_t index)
{
    if (queue->orderBy == TCBPQ_ORDER_BY_DATA)
        return queue->store[index]->data;
    else 
        return queue->store[index]->priority;
}

/* This function re-orders the heap from the index up. */
static void HeapUp(OS_tcbPriorityQueue_t* const queue, const uint32_t index) {
	if (queue->length == 0) 
		return;							// heap is empty so stop
	else if (queue->length - 1 == 0)
		return;							// node is the root element so stop
	
	int parentIndex = index / 2; 
	int childIndex  = index;
    OS_TCB_t* temp;
    
    while (ElementValue(queue, childIndex) < ElementValue(queue, parentIndex))
    {
        temp = queue->store[parentIndex];
        queue->store[parentIndex] = queue->store[childIndex];
        queue->store[childIndex] = temp;
        
        parentIndex /= 2;
        childIndex /= 2;
    }
}

static void HeapDown(OS_tcbPriorityQueue_t* const queue, const uint32_t parentNode) {
	if (queue->length <= 1)
		return;  			// heap is empty or has one node so stop
    
    uint32_t   parentIndex = parentNode;
	int32_t    leftIndex   = 2 * parentIndex + 1;
	int32_t    rightIndex  = 2 * parentIndex + 1 + 1;
	uint32_t   minIndex    = parentIndex;
	OS_TCB_t*  temp;
    
    uint_fast8_t sorting = 1;
    while (sorting)
    {	
        // Check if parent has children.
        if (leftIndex >= queue->length || leftIndex < 0)
            leftIndex = -1;
        if (rightIndex >= queue->length || rightIndex < 0)
            rightIndex = -1;
        
        if (leftIndex != -1 && ElementValue(queue, leftIndex) < ElementValue(queue, parentIndex))
            minIndex = leftIndex;
        
        if (rightIndex != -1 && ElementValue(queue, rightIndex) < ElementValue(queue, minIndex))
            minIndex = rightIndex;
        
        if (minIndex != parentIndex)
        {
            temp = queue->store[minIndex];
            queue->store[minIndex] = queue->store[parentIndex];
            queue->store[parentIndex] = temp;
            
            parentIndex = minIndex;
            leftIndex = 2 * parentIndex + 1;
            rightIndex = 2 * parentIndex + 1 + 1;
        }
        else 
        {
            sorting = 0;
        }
    }
}

/* This function returns the index at which the tcb passed as a parameter is 
found in the queue's store array. If the tcb is not found, -1 is returned. */
static int32_t Search(const OS_tcbPriorityQueue_t* const queue, 
                        const OS_TCB_t* const tcb)
{
    // Perform breadth-first search to find the tcb.
    for (uint32_t i = 0; i < queue->length; i++) 
    {   
        if (queue->store[i] == tcb)
        {
            return i;
        }
    }    
    
    return -1;
}

void OS_InitTCBPriorityQueue(OS_tcbPriorityQueue_t* const queue, 
                               OS_TCB_t** store, 
                               const uint32_t nMaxTasks,
                               const uint32_t orderBy)
{
    queue->store = store;
	queue->length = 0;    
    queue->orderBy = orderBy;
    
    // Even though this is actually a zero-based number, the paramater is not. 
    // This makes it easier for calling tasks to initialise the queue, for 
    // example, the fixed-priority scheduler can simply pass FPS_MAX_TASKS 
    // instead of FPS_MAX_TASKS - 1.
    queue->nMaxTasks = nMaxTasks - 1;
}

void OS_TCBPriorityQueueInsert(OS_tcbPriorityQueue_t* queue, OS_TCB_t* tcb)
{
    if (OS_TCBPriorityQueueFull(queue))
    {
        return;
    }
    
    // The new element is always added to the end of a heap.
	queue->store[(queue->length)++] = tcb;
	HeapUp(queue, queue->length - 1);
}

OS_TCB_t* OS_TCBPriorityQueueExtract(OS_tcbPriorityQueue_t* const queue)
{
    if (OS_TCBPriorityQueueEmpty(queue))
    {
        return 0;
    }
    
    // The root value is extracted, and the space filled by the value from the 
    // end.
	OS_TCB_t* value = queue->store[0];
	queue->store[0] = queue->store[--(queue->length)];
    queue->store[queue->length] = 0;   
	HeapDown(queue, 0);
	return value;
}

OS_TCB_t* OS_TCBPriorityQueuePeek(OS_tcbPriorityQueue_t* const queue)
{
    // Since queue is priority sorted, highest priority tcb is always at the 
    // start of the heap. 
    if (OS_TCBPriorityQueueEmpty(queue))
    {
        return 0;
    }
    
    return queue->store[0];
}

void OS_TCBPriorityQueueRemove(OS_tcbPriorityQueue_t* const queue,
                                 const OS_TCB_t* const tcb) 
{
    int32_t tcbIndex = Search(queue, tcb);
    if (tcbIndex == -1)
    {
        return;
    }
    queue->store[tcbIndex] = queue->store[--(queue->length)];
    queue->store[queue->length] = 0;
    HeapDown(queue, tcbIndex);
}

void OS_TCB_PriorityQueueReSort(OS_tcbPriorityQueue_t* const queue)
{
    HeapDown(queue, 0);
}

uint32_t OS_TCBPriorityQueueFull(OS_tcbPriorityQueue_t* const queue) 
{
    return queue->length > queue->nMaxTasks;
}

uint32_t OS_TCBPriorityQueueEmpty(OS_tcbPriorityQueue_t* const queue)
{
    return !(queue->length);
}

void OS_TCBPriorityQueuePrint(const OS_tcbPriorityQueue_t* const queue)
{
    for (uint32_t i = 0; i < queue->length; i++)
    {
        printf("%d\n", queue->store[i]->priority);
    }
}