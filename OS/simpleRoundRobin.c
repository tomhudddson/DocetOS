//#include "simpleRoundRobin.h"

//#include "stm32f3xx.h"
//#include "debugTools.h"

///* This is an implementation of an extremely simple (and inefficient!) 
//round-robin scheduler.

//An array of pointers to TCBs is declared, and when tasks are added they are 
//inserted into this array.  When tasks finish, they are removed from the array 
//(pointers are set back to zero). When the scheduler is invoked, it simply looks 
//for the next available non-zero pointer in the array, and returns it.  If there 
//are no non-zero TCB pointers in the array, a pointer to the idle task is 
//returned instead.
//	 
//The inefficiency in this implementation arises mainly from the way arrays are 
//used for list storage. If the array is large and the number of tasks is small, 
//the scheduler will spend a lot of time looking through the array each time it 
//is called. */

//static OS_TCB_t* _tasks[SRR_MAX_TASKS] = { 0 };

///* Scheduler callback function prototypes. */
//static const OS_TCB_t*  SRR_SchedulerCallback(void);
//static void  SRR_AddTaskCallback(OS_TCB_t * const tcb, const uint32_t priority);
//static void  SRR_TaskExitCallback(OS_TCB_t * const tcb);
//static void  SRR_WaitCallback(void* const reason, const uint32_t checkCode);
//static void  SRR_NotifyCallback(void* const reason);

//OS_Scheduler_t const simpleRoundRobinScheduler = {
//	.preemptive        = OS_PREEMPTIVE_SCHEDULING,
//    .type              = 2,
//	.SchedulerCallback = SRR_SchedulerCallback,
//	.AddTaskCallback   = SRR_AddTaskCallback,
//	.TaskExitCallback  = SRR_TaskExitCallback,
//    .WaitCallback      = SRR_WaitCallback,
//    .NotifyCallback    = SRR_NotifyCallback
//};

//static const OS_TCB_t * SRR_SchedulerCallback(void) 
//{
//	static int i = 0;
//	// Clear the yield flag if it's set - we simply don't care
//	OS_CurrentTCB()->state &= ~TASK_STATE_YIELD;
//	for (int j = 1; j <= SRR_MAX_TASKS; j++) 
//    {
//		i = (i + 1) % SRR_MAX_TASKS;
//		if (_tasks[i] != 0 && 
//              !IsTaskSleeping(_tasks[i], OS_ElapsedTicks()) &&
//              !IsTaskWaiting(_tasks[i]->state))
//        {
//			return _tasks[i];
//		}
//	}
//    
//	// No tasks in the list, so return the idle task
//	return OS_idleTCB_p;
//}

//static void SRR_AddTaskCallback(OS_TCB_t * const tcb, const uint32_t priority) 
//{
//	for (int i = 0; i < SRR_MAX_TASKS; i++) 
//    {
//		if (_tasks[i] == 0) 
//        {
//			_tasks[i] = tcb;
//			return;
//		}
//	}
//    
//	// If we get here, there are no free TCB slots, so the task just won't be added
//}

//static void SRR_TaskExitCallback(OS_TCB_t * const tcb) {
//	// Remove the given TCB from the list of tasks so it won't be run again
//	for (int i = 0; i < SRR_MAX_TASKS; i++) 
//    {
//		if (_tasks[i] == tcb) 
//        {
//			_tasks[i] = 0;
//		}
//	}	
//}

//static void SRR_WaitCallback(void* const reason, const uint32_t checkCode) 
//{
//    OS_CurrentTCB()->data = (uint32_t)reason;
//    
//    // Set the PendSV to invoke the scheduler.
//    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;   
//}

//static void SRR_NotifyCallback(void* const reason) 
//{
//    for (int i = 0; i < SRR_MAX_TASKS; i++)
//    {
//        if (_tasks[i] == NULL)
//            continue;
//        
//        // Currently this function only notifies tcbs in the wait state. It 
//        // needs to also notify sleeping tasks.
//        if (_tasks[i]->data == (uint32_t)reason && 
//            IsTaskWaiting(_tasks[i]->state)) 
//        {
//            _tasks[i]->state &= ~TASK_STATE_WAIT;
//            _tasks[i]->data = NULL;
//        }
//    }
//    
//    // Set the PendSV bit to invoke the scheduler.
//    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
//}
