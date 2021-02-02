#include "mutex.h"

#include "cmsis_armcc.h"
#include "os.h"
#include "debugTools.h"


void OS_InitMutex(OS_mutex_t* const mutex)
{
    mutex->tcb = 0;
    mutex->counter = 0;
    OS_InitTCBPriorityQueue(&mutex->_waitingTaskQueue, mutex->_waitingTasks, MAX_TASKS, TCBPQ_ORDER_BY_PRIORITY); 
}

void OS_MutexAquire(OS_mutex_t* const mutex)
{
    LOG(LOG_LVL_TRACE, "OS_MutexAquire.\n");
    
    OS_TCB_t*  mutexTcb  = 0;
    uint32_t   stored    = 1;
    uint32_t   checkCode = OS_GetCheckCode();
    
    while (stored == 1) 
    {
        mutexTcb = (OS_TCB_t* )__LDREXW((uint32_t* )&mutex->tcb);
        
        if (mutexTcb == 0)
        {
            // The mutex is empty so the current task may aquire it. Hence, 
            // store the current tcb in the mutex's tcb field. 
            stored = __STREXW(OS_CurrentTCB(), (uint32_t* )&mutex->tcb);  
        } 
        else if (mutexTcb != 0 && mutexTcb != OS_CurrentTCB())
        {
            // The mutex has been aquired by a different task so the current
            // task must wait.
            OS_Wait(&mutex->_waitingTaskQueue, checkCode);
            //return;
        }  
    }
    
    mutex->counter++;
}

void OS_MutexRelease(OS_mutex_t* const mutex)
{
    // Check if the mutex is currently owned. If it is not, nothing needs to be 
    // released removing the need to call the OS_Notify function.
    if (mutex->tcb == 0) 
    {
        // If a task acquires a mutex before the operating system starts,
        // the counter will be incremented, but the tcb field stored in the 
        // mutex will still be 0. This is because the _currentTcb field in 
        // os.h is initialised to 0 before OS_Start()is called. An example of 
        // this occuring is in the OS_Dalloc() function, which when called to 
        // add a task to a memory pool during OS initialisation, aqcuires the 
        // mutex, which will be 0. Hence the value stored in the mutex's tcb 
        // field will also be zero. 
        //
        // To prevent this, the mutex's counter is set to 0 if the tcb field is 
        // 0, regardless of if the current tcb is 0 or the mutex is simply free.
        mutex->counter = 0;
        return; 
    }
        
    // Check if the task that is calling this function owns the mutex.
    if (mutex->tcb != OS_CurrentTCB()) { return; }
    
    mutex->counter--;
    
    // If there are no counts of this mutex, free its tcb field so it can be 
    // aquired by another task, and also notify waiting tasks that the 
    // mutex is free.
    if (mutex->counter <= 0)
    {        
        mutex->tcb = 0;
        OS_Notify(&mutex->_waitingTaskQueue);       
    }
    
    
}
