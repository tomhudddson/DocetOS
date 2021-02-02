#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <stddef.h>

#define OS_SCHEDULER_PRIORITY_LVL_1     1
#define OS_SCHEDULER_PRIORITY_LVL_2     2
#define OS_SCHEDULER_PRIORITY_LVL_3     3
#define OS_SCHEDULER_PRIORITY_LVL_4     4
#define OS_SCHEDULER_PRIORITY_LVL_5     5
#define OS_SCHEDULER_PRIORITY_LVL_NONE  20

#define MAX_TASKS 10

/** 
* @brief Describes a single stack frame, as found at the top of the stack of a 
*   task that is not currently running.  Registers r0-r3, r12, lr, pc and psr 
*   are stacked automatically by the CPU on entry to handler mode.  Registers 
*   r4-r11 are subsequently stacked by the task switcher. That's why the order 
*   is a bit weird. */
typedef struct s_StackFrame {
	volatile uint32_t r4;
	volatile uint32_t r5;
	volatile uint32_t r6;
	volatile uint32_t r7;
	volatile uint32_t r8;
	volatile uint32_t r9;
	volatile uint32_t r10;
	volatile uint32_t r11;
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr;
	volatile uint32_t pc;
	volatile uint32_t psr;
} OS_StackFrame_t;

/** 
* @brief Struct containing a task control block, which is a 'task'.
*/
typedef struct {
	// Task stack pointer. It's important that this is the first entry in the
    // structure, so that a simple double-dereference of a TCB pointer yields a 
    // stack pointer. 
	void * volatile sp;
    
	// This field is intended to describe the state of the thread - whether it's
    // yielding, runnable, or whatever.
	uint32_t volatile state;
    
    // This field stores the priority level of the task. A higher priority means
    // the task will be run sooner than those with lower priority levels by
    // the priority scheduler. 
	uint32_t volatile priority;
    
	uint32_t volatile data;
} OS_TCB_t;

/* Constants that define bits in a thread's 'state' field. */
#define TASK_STATE_YIELD   (1UL << 0)  
#define TASK_STATE_SLEEP   (1UL << 1)  
#define TASK_STATE_WAIT    (1UL << 2)  

/**
* @brief Determine whether a task is in the wait state.
* @param taskState The 'state' field of the task in question.
* @return 1 if the task is waiting, 0 if it is not.
*/
uint32_t IsTaskWaiting(const uint32_t taskState);

/**
* @brief Determine whether a task is in the sleep state. If the task's sleep
* time has elapsed, it will be removed from the sleep state.
* @param task The task to in question.
* @param elapsedTicks The number of elapsed ticks since the start of the 
*   operating system. This can be retrieved by calling OS_ElapsedTicks().
* @return 1 if the task is in the sleep state, 0 if it is not in the sleep 
* state, or has been removed from the sleep state.
*/
uint32_t IsTaskSleeping(OS_TCB_t* const task, const uint32_t elapsedTicks);

#endif /* _TASK_H_ */
