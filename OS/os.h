#ifndef _OS_H_
#define _OS_H_

#define OS_PREEMPTIVE_SCHEDULING      1
#define OS_NON_PREEMPTIVE_SCHEDULING  0

#define OS_SCHEDULER_TYPE_FPS 1
#define OS_SCHEDULER_TYPE_SRR 2

#include "task.h"
#include "itc_queue.h"
#include "tcb_priority_queue.h"

/********************/
/* Type definitions */
/********************/

/** 
* @brief A set of numeric constants giving the appropriate SVC numbers for 
*   various callbacks. If this list doesn't match the SVC dispatch table 
*   in os_asm.s, BIG TROUBLE will ensue. 
*/
enum OS_SVC_e {
	OS_SVC_ENABLE_SYSTICK = 0x00,
	OS_SVC_ADD_TASK,
	OS_SVC_EXIT,
	OS_SVC_YIELD,
	OS_SVC_SCHEDULE,
    OS_SVC_WAIT,
    OS_SVC_NOTIFY,
    OS_SVC_FORCE_PRINT
};

/**
* @brief A structure to hold callbacks for a scheduler, plus a 'preemptive' 
*   flag. 
*/
typedef struct {
	uint_fast8_t preemptive;
	OS_TCB_t const * (* SchedulerCallback)(void);
	void (* AddTaskCallback)(OS_TCB_t* const newTask, const uint32_t priority);
	void (* TaskExitCallback)(OS_TCB_t* const task);
    void (* InitCallback)(void);
    void (* WaitCallback)(OS_tcbPriorityQueue_t* const waitingTaskQueue, OS_TCB_t* tcb);
    void (* NotifyCallback)(OS_tcbPriorityQueue_t* const waitingTaskQueue);
    void (* SleepCallback)(OS_TCB_t* const tcb, const uint32_t currentTime, const uint32_t time);
} OS_Scheduler_t;

/***************************/
/* OS management functions */
/***************************/

/**
* @brief Initialises the OS. Must be called before OS_start(). The argument is a 
*        pointer to an OS_Scheduler_t structure. 
*/
void OS_Init(OS_Scheduler_t const * scheduler);

/** 
* @brief Starts the OS kernel. Never returns. 
*/
void OS_Start(void);

/**
* @brief Returns a pointer to the TCB of the currently running task. 
*/
OS_TCB_t * OS_CurrentTCB(void);

/** 
* @brief Returns the number of elapsed systicks since the last reboot 
*   (modulo 2^32). 
*/
uint32_t OS_ElapsedTicks(void);

/**
* @brief Returns the value of the check code.
*/
uint32_t OS_GetCheckCode(void);

/** 
* @brief Returns the type of scheduler currently in use.
* @return OS_SCHEDULER_TYPE_SRR if the simple round robin scheduler is in use.
* @return OS_SCHEDULER_TYPE_FPS if the fixed priority scheduler is in use.
*/
uint32_t OS_GetSchedulerType(void);

/******************************************/
/* Task creation and management functions */
/******************************************/

/** 
* @brief Initialises a task control block (TCB) and its associated stack. The 
*   stack is prepared with a frame such that when this TCB is first used in a 
*   context switch, the given function will be executed. If and when the 
*   function exits, a SVC call will be issued to kill the task, and a callback
*   will be executed.
* @param TCB Pointer to a TCB structure to initialise.
* @param stack Pointer to the TOP OF a region of memory to be used as a stack 
*   (stacks are full descending). Note that the stack MUST be 8-byte aligned.  
*   This means if (for example) malloc() is used to create a stack, the result 
*   must be checked for alignment, and then the stack size must be added to the 
*   pointer for passing to this function.
* @param func Pointer to the function that the task should execute.
* @param data Void pointer to data that the task should receive. 
*/
void OS_InitialiseTCB(OS_TCB_t * TCB, 
                      uint32_t * const stack, 
                      void (* const func)(void const * const), 
                      void const * const data);

/**
* @brief SVC delegate to add a task. 
* @param tcb The tcb to add.
* @param priority The priority that the task will hold. The list of priority 
*   levels can be found in task.h.                      
**/
void __svc(OS_SVC_ADD_TASK) OS_AddTask(OS_TCB_t const * const tcb, 
                                       const uint32_t priority);

/**
* @brief SVC delegate to put the current task into the wait state.
* @param reason The reason code for going into the wait state. This will be 
*   later used to remove the task from the wait state.
* @param checkCode The check code to ensure the state of the system has not been
*   changed, or this delegate has not been interrupted, since this function was 
*   called.                      
*/                      
void __svc(OS_SVC_WAIT) OS_Wait(OS_tcbPriorityQueue_t* const waitingTaskQueue, 
                                  const uint32_t checkCode);

/**
* @brief SVC delegate to notify the current task. 
* @param reason The reason code for notifying the task.
*/
void __svc(OS_SVC_NOTIFY) OS_Notify(OS_tcbPriorityQueue_t* const waitingTaskQueue);

void OS_Sleep(const uint32_t time);

/************************/
/* Scheduling functions */
/************************/

/**
* @brief SVC delegate to yield the current task. 
*/
void __svc(OS_SVC_YIELD) OS_Yield(void);

/****************/
/* Declarations */
/****************/

/** 
* @brief Idle task TCB.
*/
extern OS_TCB_t const * const OS_idleTCB_p;

#endif /* _OS_H_ */

