#include "os.h"

#include <stdlib.h>
#include <string.h>
#include "stm32f3xx.h"
#include "cmsis_armcc.h"

#include "os_internal.h"
#include "debugTools.h"

__align(8)
/* Idle task stack frame area and TCB.  The TCB is not declared const, to ensure
   that it is placed in writable memory by the compiler.  The pointer to the TCB 
   _is_ declared const, as it is visible externally - but it will still be 
   writable by the assembly-language context switch. */
static OS_StackFrame_t const volatile _idleTaskSF;
static OS_TCB_t OS_idleTCB = { (void *)(&_idleTaskSF + 1), 0, 0, 0 };
OS_TCB_t const * const OS_idleTCB_p = &OS_idleTCB;

/* Total elapsed ticks. */
static volatile uint32_t _ticks = 0;

static OS_Scheduler_t const * _scheduler = 0;

/* A check code which can be obtained prior to starting an operation, and 
   checked to ensure that it hasn't changed after the operation has finished. If 
   the check code has changed, then the state of the system is different to when 
   the operation began. Therefore, the operation must be done again. */
static volatile uint32_t _checkCode;

/* GLOBAL: Holds pointer to current TCB.  DO NOT MODIFY, EVER. */
OS_TCB_t * volatile _currentTCB = 0;

OS_TCB_t * OS_CurrentTCB() 
{
	return _currentTCB;
}

uint32_t OS_ElapsedTicks() 
{
	return _ticks;
}

/* IRQ handler for the system tick. Schedules PendSV asynchronously. */
void SysTick_Handler(void) 
{
	_ticks = _ticks + 1;
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* SVC handler for OS_yield().  Sets the TASK_STATE_YIELD flag and schedules 
   PendSV. */
void _svc_OS_yield(void)
{
	_currentTCB->state |= TASK_STATE_YIELD;
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* SVC handler for OS_schedule(). Simply schedules PendSV. */
void _svc_OS_schedule(void) 
{
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* Sets up the OS by storing a pointer to the structure containing all the 
   callbacks. Also establishes the system tick timer and interrupt if preemption 
   is enabled. */
void OS_Init(OS_Scheduler_t const * scheduler) 
{
	_scheduler = scheduler;
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;
//    *((uint32_t volatile *)0xE000ED14) |= (1 << 9); // Set STKALIGN
	ASSERT(_scheduler->SchedulerCallback);
	ASSERT(_scheduler->AddTaskCallback);
	ASSERT(_scheduler->TaskExitCallback);
    ASSERT(_scheduler->WaitCallback);
    ASSERT(_scheduler->NotifyCallback);
    ASSERT(_scheduler->WaitCallback);
    
    _checkCode = 0;
}

void OS_Start()
{
	ASSERT(_scheduler);
	// This call never returns (and enables interrupts and resets the stack)
	_task_init_switch(OS_idleTCB_p);
}

void OS_InitialiseTCB(OS_TCB_t * TCB, uint32_t * const stack, 
                      void (* const func)(void const * const), 
                      void const * const data) 
{
	TCB->sp = stack - (sizeof(OS_StackFrame_t) / sizeof(uint32_t));
	TCB->priority = TCB->state = TCB->data = 0;
	OS_StackFrame_t *sf = (OS_StackFrame_t *)(TCB->sp);
	memset(sf, 0, sizeof(OS_StackFrame_t));
    
	// By placing the address of the task function in pc, and the address of 
    // _OS_task_end() in lr, the task function will be executed on the first 
    // context switch, and if it ever exits, _OS_task_end() will be called 
    // automatically. 
	sf->lr = (uint32_t)_OS_task_end;
	sf->pc = (uint32_t)(func);
	sf->r0 = (uint32_t)(data);
	sf->psr = 0x01000000;  // Set the thumb bit to avoid a big steaming fault 
}

/* Function that's called by a task when it ends (the address of this function 
   is inserted into the link register of the initial stack frame for a task).  
   Invokes an SVC call (see os_internal.h); the handler is _svc_OS_task_exit() 
   (see below). */
void _OS_task_end(void) {
    // DO NOT STEP OUT OF THIS FUNCTION when debugging!  PendSV must be allowed 
    // to run and switch tasks.  A hard fault awaits if you ignore this. If you 
    // want to see what happens next, or debug something, set a breakpoint at 
    // the start of PendSV_Handler (see os_asm.s) and hit 'run'.
    
	_OS_task_exit();
}

/* SVC handler to enable systick from unprivileged code. */
void _svc_OS_enable_systick(void) {
	if (_scheduler->preemptive) {
		SystemCoreClockUpdate();
		SysTick_Config(SystemCoreClock / 1000);
		NVIC_SetPriority(SysTick_IRQn, 0x10);
	}
}

/* SVC handler to add a task. Invokes a callback to do the work. */
void _svc_OS_AddTask(_OS_SVC_StackFrame_t const * const stack) 
{
	// The TCB pointer is on the stack in the r0 position, having been passed as
    // an argument to the SVC pseudo-function. SVC handlers are called with the
    // stack pointer in r0 (see os_asm.s) so the stack can be interrogated to 
    // find the TCB pointer. 
	_scheduler->AddTaskCallback((OS_TCB_t *)stack->r0, stack->r1);
}

/* SVC handler to invoke the scheduler (via a callback) from PendSV. */
OS_TCB_t const * _OS_scheduler() {
	return _scheduler->SchedulerCallback();
}

/* SVC handler that's called by _OS_task_end when a task finishes.  Invokes the
   task end callback and then queues PendSV to call the scheduler. */
void _svc_OS_task_exit(void) {
	_scheduler->TaskExitCallback(_currentTCB);
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* SVC hander that's called by OS_Wait. It atomically loads the current TCB,
sets its state to the 'wait' state, and invokes the scheduler's wait callback
function. */
void _svc_OS_Wait(const _OS_SVC_StackFrame_t* const stack) 
{  
    OS_TCB_t* atomTcb;
    
    do 
    {
        atomTcb = (OS_TCB_t* )__LDREXW((uint32_t* )&_currentTCB);
        if (stack->r1 != OS_GetCheckCode())
        {
            // Call to notify must have occured abort wait;
            return;
        }
        
        atomTcb->state |= TASK_STATE_WAIT;
    } while (__STREXW(atomTcb, (uint32_t* )&_currentTCB));
    
    _scheduler->WaitCallback((OS_tcbPriorityQueue_t* )stack->r0, atomTcb);
}

/* SVC handler that's called by OS_Notify. */
void _svc_OS_Notify(const _OS_SVC_StackFrame_t* const stack) 
{
    _checkCode++;
    __CLREX();
    _scheduler->NotifyCallback((OS_tcbPriorityQueue_t* )stack->r0);
}

/* This funtion atomically loads the current tcb and the current elapsed
ticks, then sets the current tcb's state to the 'sleep' state and sets the 
tcb's data field to the number of elapsed ticks for when the task is due to 
wake. */
void OS_Sleep(const uint32_t time)
{
    uint32_t atomTime;
    OS_TCB_t* atomTcb;
    
    do 
    {
        atomTcb = (OS_TCB_t* )__LDREXW((uint32_t* )&_currentTCB);
        atomTime = OS_ElapsedTicks();
        
        atomTcb->state |= TASK_STATE_SLEEP;
        atomTcb->data = atomTime + time;
    } while (__STREXW(atomTcb, (uint32_t* )&_currentTCB));
    
    _scheduler->SleepCallback(atomTcb, time, atomTime);
}

uint32_t OS_GetCheckCode(void)
{
    return _checkCode;
}

