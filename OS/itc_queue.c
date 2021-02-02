#include "itc_queue.h"

#include <string.h>

#include "os.h"

#include "debugTools.h"

/* This function initailises the data and dest fields to 0, and initialises
the queues mutex and semaphore. Note, a location in the msgbuf array is 
deemed to be empty only when the dest field is 0. */
void OS_InitITCQueue(OS_itcQueue_t* const queue)
{
    for (int i = 0; i < ITC_MAX_MSGS; i++)
    {
        queue->msgbuf[i].data = 0;
        queue->msgbuf[i].dest = 0;
    }
    
    OS_InitMutex(&queue->mux);
    OS_InitSemaphore(&queue->sem, ITC_MAX_MSGS);
}

void OS_ITCSendMsg(OS_itcQueue_t* const queue, 
                    const void* const data, 
                    const size_t dataSz,
                    OS_TCB_t* const dest)
{
    // Check if the queue is full here as there is no need to continue to try 
    // and acquire the mutex if it is. In fact, if the mutex is acquired even
    // though the msgbuf is full, then no message will be added to the queue,
    // but a resource from the semaphore will still be acquired at the end of 
    // the function - bad.
    if (OS_SemaphoreFull(&queue->sem))
    {
        // There is no empty location in msgbuf, so make the calling task wait.
        OS_SemaphoreWait(&queue->sem, OS_GetCheckCode());        
    }
    
    OS_MutexAquire(&queue->mux);
    
    // Find an empty place in the queue and insert the message there.
    for (uint32_t i = 0; i < ITC_MAX_MSGS; i++) 
    {
        if (queue->msgbuf[i].dest == 0) 
        {
            queue->msgbuf[i].dataSz = dataSz;
            memcpy(&queue->msgbuf[i].data, &data, dataSz);           
            queue->msgbuf[i].dest = dest;
            break;
        }
    }
        
    OS_MutexRelease(&queue->mux);  
    OS_SemaphoreAquire(&queue->sem); 
}

void OS_ITCReadMsg(OS_itcQueue_t* const queue, void** const data)
{
    // Check if the queue is empty here and if it is, make the calling task
    // wait. If the calling task was able to acquire the semaphore, even though
    // the msgbuf is empty, then no message would be found but a resource 
    // would still be released back to the semaphore at the end of the function,
    // which is bad.
    if (OS_SemaphoreEmpty(&queue->sem))
    {
        OS_SemaphoreWait(&queue->sem, OS_GetCheckCode());
    }
    
    OS_MutexAquire(&queue->mux);
    
    for (uint32_t i = 0; i < ITC_MAX_MSGS; i++)
    {
        if (queue->msgbuf[i].dest == OS_CurrentTCB())
        {
            // Copy the message data from the queue to the data output 
            // paramater, and remove the message from the message queue.
            memcpy(data, &queue->msgbuf[i].data, queue->msgbuf[i].dataSz);
            queue->msgbuf[i].data = 0;
            queue->msgbuf[i].dest = 0;
            break;
        }            
    }
    
    OS_MutexRelease(&queue->mux);
    OS_SemaphoreRelease(&queue->sem);
}

uint32_t OS_ITCHasMsg(OS_itcQueue_t* const queue)
{
    for (uint32_t i = 0; i < ITC_MAX_MSGS; i++)
    {
        if (queue->msgbuf[i].dest == OS_CurrentTCB())
        {
            return 1;
        }
    }
   
    return 0;
}

void OS_ITCPrintQueue(OS_itcQueue_t* const queue)
{
    for (int i =0; i < ITC_MAX_MSGS; i++)
    {
        LOG(LOG_LVL_MSG, "Queue[%d]: data = %d, dest = %x\n", i, (int)queue->msgbuf[i].data, (uint32_t)queue->msgbuf[i].dest);
    }
    
    LOG_OUTPUT_RAW("\n");
}

