#ifndef FIXED_PRIORITY_SCHEDULER
#define FIXED_PRIORITY_SCHEDULER

#include "os.h"

/*
The fixed-priority scheduler schedules tasks to be executed in order of their 
priority, with the highest priority being priority level 1, and the lowest level 
being priority level 5 (definitions can be found in task.h). If there are 
multiple tasks with equal priority, they are scheduled first-come, first-served,
however, there is no guarentee that those tasks will be scheduled in the same 
order they were added. To ensure tasks are executed in a specific order, take
advantage of the priority levels.

There are a maximum number of tasks that the fixed-priority scheduler can 
manage, FPS_MAX tasks (defined in task.h). If the scheduler is full, and more 
tasks are added using OS_Add(), they will simply not be added and not scheduled
and executed.
*/

extern OS_Scheduler_t const fixedPriorityScheduler;

/**
* @brief Initialise the fixed priority scheduler. This must be called before 
*   OS_Start().
*/
void OS_InitFPS(void);

#endif  // FIXED_PRIORITY_SCHEDULER
