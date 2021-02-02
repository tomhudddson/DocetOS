#include "fixedPriorityScheduler.h"

#include "stm32f3xx.h"

#include "tcb_priority_queue.h"
#include "debugTools.h"

/*
All running tasks are stored in a priority-queue, _runningTasksQueue. Only tasks 
inside this queue will ever be scheduled and executed. There is a second 
priority-queue, _sleepingTasksQueue, which contains all tasks in the 'sleep' 
state. When a task is put to sleep with OS_Sleep(), it gets moved into the 
sleeping tasks queue until it has slept for the required amount of time, at 
which point it is moved back into _runningTasksQueue so it can be scheduled and 
executed again.

Similarly, when a task is put into the 'wait' state, it gets removed from 
_runningTasksQueue and moved into another priority-queue which stores waiting
tasks. Each object, e.g. mutex, semaphore, etc... carries their own waiting 
tasks queue. When one of these puts the current task into the 'wait' state, 
the task is removed from _runningTasksQueue and put into their own waiting task
queue. Once the task is due to be woken and OS_Notify() is called, then the 
waiting task is moved from the object's waiting task to and back into the 
_runningTasksQueue.

The _runningTasksQueue ensures the highest-priority tasks will always be at the 
front. However, due to its implementation using a heap, the order of which 
the highest-priority tasks are in is not guarenteed. For example, if three tasks
were added in order 1, 2, 3, and all had priority level 1, then it is not 
certain that they will execute in that order. The _sleepingTasksQueue ensures 
the task with the lowest time left to sleep is always at the front. 
*/


static OS_tcbPriorityQueue_t  _runningTasksQueue;
static OS_tcbPriorityQueue_t  _sleepingTasksQueue;

/* These store the tasks used in the _runningTasksQueue and 
_sleepingTasksQueue. */
static OS_TCB_t*  _runningTasks[MAX_TASKS];
static OS_TCB_t*  _sleepingTasks[MAX_TASKS];

/* Scheduler callback function prototypes. */
static const OS_TCB_t*  FPS_SchedulerCallback(void); 
static void  FPS_AddTaskCallback(OS_TCB_t* const newTask, const uint32_t priority);
static void  FPS_TaskExitCallback(OS_TCB_t* const task);
static void  FPS_TaskWaitCallback(OS_tcbPriorityQueue_t* const waitingTaskQueue, OS_TCB_t* const tcb);
static void  FPS_TaskNotifyCallback(OS_tcbPriorityQueue_t* const waitingTaskQueue);
static void  FPS_TaskSleepCallback(OS_TCB_t* const tcb, const uint32_t currentTime, const uint32_t time);

OS_Scheduler_t const fixedPriorityScheduler = 
{
    .preemptive        = OS_PREEMPTIVE_SCHEDULING,
    .SchedulerCallback = FPS_SchedulerCallback,
    .AddTaskCallback   = FPS_AddTaskCallback,
    .TaskExitCallback  = FPS_TaskExitCallback,
    .WaitCallback      = FPS_TaskWaitCallback,
    .NotifyCallback    = FPS_TaskNotifyCallback,
    .SleepCallback     = FPS_TaskSleepCallback
};

void OS_InitFPS(void)
{
    OS_InitTCBPriorityQueue(&_runningTasksQueue, _runningTasks, MAX_TASKS, TCBPQ_ORDER_BY_PRIORITY);
    OS_InitTCBPriorityQueue(&_sleepingTasksQueue, _sleepingTasks, MAX_TASKS, TCBPQ_ORDER_BY_DATA);
}

/* This function determiens whether there are tasks in _sleepingTasksQueue
that need removing from the 'sleep' state and into _runningTasks. If there are,
it will do so. */
static void UpdateSleepingTasks()
{
    uint32_t ticks = OS_ElapsedTicks();
    OS_TCB_t* extracted = 0;
    
    while (ticks > OS_TCBPriorityQueuePeek(&_sleepingTasksQueue)->data)
    {
        extracted = OS_TCBPriorityQueueExtract(&_sleepingTasksQueue);
        extracted->data = 0;  // clear any data related to sleeping
        OS_TCBPriorityQueueInsert(&_runningTasksQueue, extracted);
    }
}

const OS_TCB_t* FPS_SchedulerCallback(void)
{
    if (!OS_TCBPriorityQueueEmpty(&_sleepingTasksQueue)) 
    {
        UpdateSleepingTasks();
    }
    
        
    OS_TCB_t* tcb = OS_TCBPriorityQueuePeek(&_runningTasksQueue);
    if (!tcb)
    {
        // No task found in the running task queue, so return the idle task.
        return OS_idleTCB_p;
    }
    
    // Return the highest priority task.
    return tcb;
}

void FPS_AddTaskCallback(OS_TCB_t* const newTask, const uint32_t priority)
{
    newTask->priority = priority;
    OS_TCBPriorityQueueInsert(&_runningTasksQueue, newTask);
}

void FPS_TaskExitCallback(OS_TCB_t* const task)
{
    // Task no longer should be executed so remove it from the running tasks 
    // queue.
    OS_TCBPriorityQueueExtract(&_runningTasksQueue);
}

void FPS_TaskWaitCallback(OS_tcbPriorityQueue_t* const waitingTaskQueue,
                            OS_TCB_t* const tcb) 
{   
    //OS_TCB_t* tcb = OS_CurrentTCB();
    
    OS_TCBPriorityQueueRemove(&_runningTasksQueue, tcb);
    
    // Insert removed task into the calling objects waiting task queue.
    OS_TCBPriorityQueueInsert(waitingTaskQueue, tcb);
    
    // Set the PendSV to invoke the scheduler.
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;     
}

void FPS_TaskNotifyCallback(OS_tcbPriorityQueue_t* const waitingTaskQueue)
{       
    // The task to be notified in the waiting task queue will always be at the 
    // front, so simply extract the front task out of the waiting task queue
    // and insert it back into the running tasks.
    OS_TCB_t* tcb = OS_TCBPriorityQueueExtract(waitingTaskQueue);
    if (!tcb)
    {
        // Queue is empty for some reason, so return.
        return;
    }
    
    OS_TCBPriorityQueueInsert(&_runningTasksQueue, tcb);
    
    // Set the PendSV bit to invoke the scheduler.
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}


void FPS_TaskSleepCallback(OS_TCB_t* const tcb, const uint32_t currentTime, const uint32_t time) 
{    
    // Remove the tcb from the running tasks queue so it does not get executed.
    OS_TCBPriorityQueueRemove(&_runningTasksQueue, tcb);
    
    // Now insert it into the sleeping tasks queue.
    OS_TCBPriorityQueueInsert(&_sleepingTasksQueue, tcb);
    
    OS_Yield();
}

