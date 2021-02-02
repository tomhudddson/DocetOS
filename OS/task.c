#include "task.h"

uint32_t IsTaskWaiting(const uint32_t taskState)
{   
    return taskState & TASK_STATE_WAIT;
}

uint32_t IsTaskSleeping(OS_TCB_t* const task, const uint32_t elapsedTicks)
{
    uint8_t asleep = (task->state & TASK_STATE_SLEEP) ? 1 : 0;
    
    if (asleep && elapsedTicks > task->data)
    {
        // The sleeping time for this task has elapsed so is set to not 
        // sleeping now. This will allow the task to be executed in the 
        // scheduler callback function.
        task->state &= ~TASK_STATE_SLEEP;
        task->data = NULL;                 // clear data, we do not have a use
                                           //     anymore.
        return 0;
    } 
    
    return asleep;
}