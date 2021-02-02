#include "memory.h"

#include "os.h"
#include "mutex.h"
#include "semaphore.h"

void OS_InitMempool(OS_mempool_t* const pool,
                      const size_t blockSz,
                      const size_t nBlocks,
                      void** elements)
{   
    pool->head = 0;
    pool->blockSz = blockSz;
    pool->nBlocks = nBlocks;
    
    // Loop through the elements array and add the address of each element
    // to the memory pool.
    for (uint32_t i = 0; i < nBlocks; i++)
    {
        void* item = (char* )&elements[0] + (blockSz * i);
        OS_MempoolAdd(pool, item);
    }
    
    OS_InitMutex(&pool->mux);
    OS_InitSemaphore(&pool->sem, nBlocks);
}

void* OS_Malloc(OS_mempool_t* const pool)
{
    // If all available blocks of memory have been allocated there is no point 
    // in attempting to acquire the mutex and allocate memory. Therefore, just 
    // put the calling task into the wait state if the pool is empty.
    if (OS_SemaphoreFull(&pool->sem))
    {
        OS_SemaphoreWait(&pool->sem, OS_GetCheckCode());
    }

 
    
    OS_MutexAquire(&pool->mux);
    
    // Return the head of the list of blocks and update the head pointer.
    void* data = pool->head->data;
	pool->head = pool->head->next;
    
    OS_MutexRelease(&pool->mux);
    OS_SemaphoreAquire(&pool->sem);   
    
    return data;
}

void OS_Dalloc(OS_mempool_t* const pool, void* const item) 
{ 
    OS_MutexAquire(&pool->mux);
    
    OS_block_t* block = item;
	block->data = item;
	block->next = pool->head;
	
	pool->head = block->data;

    OS_MutexRelease(&pool->mux);
    OS_SemaphoreRelease(&pool->sem);
}
