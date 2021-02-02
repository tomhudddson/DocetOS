#include "demonstration_tasks.h"

#include "os.h"
#include "memory.h"
#include "sleep.h"
#include "debugTools.h"

typedef struct s_TestPacket
{
    uint32_t id;
    char c;
} testPacket_t;

#ifdef DEMO_MP
#define DEMO_MP_FULL_N_PACKETS 3
static OS_TCB_t _mpFullTcb1, _mpFullTcb2;
static OS_mempool_t _mpFullPool;
static testPacket_t _elements[DEMO_MP_FULL_N_PACKETS];
testPacket_t* p1 = 0;
testPacket_t* p2 = 0;
testPacket_t* p3 = 0;
testPacket_t* p4 = 0;
testPacket_t* p5 = 0;

static void DemoMPFullTask1(void const* const args)
{    
    p1 = OS_Malloc(&_mpFullPool);
    p1->id = 5;
    p1->c = 'G';
    
    p2 = OS_Malloc(&_mpFullPool);
    p2->id = 1;
    p2->c = 'P';
    
    p3 = (testPacket_t* )OS_Malloc(&_mpFullPool);
    p3->id = 11;
    p3->c = 'V';
    
    LOG_OUTPUT("p1 (id = %d, c = %c) at address %p\n", p1->id, p1->c, p1);
    LOG_OUTPUT("p2 (id = %d, c = %c) at address %p\n", p2->id, p2->c, p2);
    LOG_OUTPUT("p3 (id = %d, c = %c) at address %p\n", p3->id, p3->c, p3);
    
    p4 = (testPacket_t* )OS_Malloc(&_mpFullPool);
    p4->id = 100;
    p4->c = 'L';
    LOG_OUTPUT("p4 (id = %d, c = %c) at address %p\n", p4->id, p4->c, p4);
    
    p5 = (testPacket_t* )OS_Malloc(&_mpFullPool);
    p5->id = 100;
    p5->c = 'L';
    LOG_OUTPUT("p5 (id = %d, c = %c) at address %p\n", p5->id, p5->c, p5);

}

static void DemoMPFullTask2(void const* const args) 
{
    OS_Sleep(3000);
    OS_Dalloc(&_mpFullPool, p1);
    
    OS_Sleep(3000);
    OS_Dalloc(&_mpFullPool, p3);
}
#endif

void DemoInitMPTasks(void) 
{
#ifdef DEMO_MP
    __align(8)
    static uint32_t stack1[64], stack2[64];
    
    OS_InitialiseTCB(&_mpFullTcb1, stack1 + 64, DemoMPFullTask1, NULL);
    OS_InitialiseTCB(&_mpFullTcb2, stack2 + 64, DemoMPFullTask2, NULL);
    OS_AddTask(&_mpFullTcb1, OS_SCHEDULER_PRIORITY_LVL_1);
    OS_AddTask(&_mpFullTcb2, OS_SCHEDULER_PRIORITY_LVL_2);
        
    OS_InitMempool(&_mpFullPool, sizeof(testPacket_t), DEMO_MP_FULL_N_PACKETS, (void* )&_elements);
    //for (uint32_t i = 0; i < DEMO_MP_FULL_N_PACKETS; i++) 
    //{
    //    OS_MempoolAdd(&_mpFullPool, &elements[i]);
    //}
#endif
}