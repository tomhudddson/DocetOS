#include "semaphore.h"

#include "cmsis_armcc.h"

#include "os.h"
#include "debugTools.h"

void OS_InitSemaphore(OS_sem_t* const sem, const size_t nResources)  
{
    sem->counter     = nResources;
    sem->nResources  = nResources;
    OS_InitTCBPriorityQueue(&sem->_waitingTasksQueue, sem->_waitingTasks, MAX_TASKS, TCBPQ_ORDER_BY_PRIORITY);
}

void OS_SemaphoreAquire(OS_sem_t* const sem)
{
    uint32_t  atomSemCount  = 0;
    uint32_t  checkCode     = OS_GetCheckCode();
    uint32_t  wasEmpty      = 0;
    
    do 
    {
        atomSemCount = __LDREXW((uint32_t* )&sem->counter);
        
        wasEmpty = (atomSemCount >= sem->nResources) ? 1 : 0;
        
        if (atomSemCount <= 0) 
        {
            OS_Wait(&sem->_waitingTasksQueue, checkCode);
        }
        else 
        {
            atomSemCount--;
        }
    } while (__STREXW(atomSemCount, &sem->counter));
    
    if (wasEmpty)
    {
        OS_Notify(&sem->_waitingTasksQueue);
    }
}

void OS_SemaphoreRelease(OS_sem_t* const sem) 
{
    uint32_t atomCounter = 0;
    
    do 
    {
        atomCounter = __LDREXW(&sem->counter);
        
        if (atomCounter + 1 > sem->nResources) 
        {
            // Clear the exclusive flag since __STREXW() is not getting 
            // executed so the exclusive access flag will not be removed
            // automatically.
            __CLREX();  
            return;
        }
        
        atomCounter++;
    } while (__STREXW(atomCounter, &sem->counter));
    
    // Safety check to prevent a task repeatedly calling this function and 
    // emptying the semaphore beyond its limit. If this did occur, more 
    // resources would be available to tasks than initially specified.
    //if (sem->counter + 1 > sem->nResources) { return; }
    
    //sem->counter++;
    if (atomCounter > 0) 
    {
        // There are free resources available now so notify any tasks that are
        // waiting to aquire the resource.
        OS_Notify(&sem->_waitingTasksQueue);
    }
}

uint32_t OS_SemaphoreGetCount(OS_sem_t* const sem)
{
    if (sem == NULL) { return 0; }
    
    uint32_t atomSemCount = 0;
    
    do 
    {
        atomSemCount = __LDREXW((uint32_t* )&sem->counter);
    } while (__STREXW(atomSemCount, &sem->counter) == 1);
    
    return atomSemCount;
}

uint32_t OS_SemaphoreGetNResources(OS_sem_t* const sem) 
{
    if (sem == NULL) { return 0; }
    
    uint32_t atomNResources = 0;
    
    do
    {
        atomNResources = __LDREXW(&sem->nResources);
    } while (__STREXW(atomNResources, &sem->nResources) == 1);
    
    return atomNResources;
}

uint32_t OS_SemaphoreFull(OS_sem_t* const sem)
{
    return OS_SemaphoreGetCount(sem) <= 0;
}

uint32_t OS_SemaphoreEmpty(OS_sem_t* const sem) 
{
    return OS_SemaphoreGetCount(sem) >= OS_SemaphoreGetNResources(sem); 
}

/*
This function exists to prevent a getter function for the waiting tasks queue.
Because a task releasing a resource to an empty semaphore doesn't get moved
to the waiting queue, there must be a way to move a task to that queue 
externally. This function does that and prevents the need for a function that
returns the waiting task queue, which is very dangerous.
*/
void OS_SemaphoreWait(OS_sem_t* const sem, const uint32_t checkCode)
{
    OS_Wait(&sem->_waitingTasksQueue, checkCode);
}

void OS_SemaphoreNotify(OS_sem_t* const sem)
{
    OS_Notify(&sem->_waitingTasksQueue);
}
