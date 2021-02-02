#include "sleep.h"

#include "os.h"
#include "tcb_priority_queue.h"

//void OS_Sleep(uint32_t time)
//{
//    OS_CurrentTCB()->data = OS_ElapsedTicks() + time;
//    OS_CurrentTCB()->state |= TASK_STATE_SLEEP;
//    OS_Yield();
//}
