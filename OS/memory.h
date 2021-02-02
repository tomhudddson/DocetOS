#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

#include "mutex.h"
#include "semaphore.h"

/**
* @brief This struct contains a single block of memory in the memory pool. These
*   are the blocks that make up the memory pool linked list.
*/
typedef struct s_Block
{
    // The data stored in this block of memory when allocated. 
	void* data;
    
    // Pointer to the next block of memeory in the memory pool.
	struct  s_Block* next;
} OS_block_t;

/**
* @brief This stuct contains a single memory pool. The memory pool is 
*   uses a linked list of OS_block_t nodes. There is no simultaneous access
*   to this memory pool as this could corrupt it. Also note, that this memory
*   pool only supports allocated data of the same type. To allocate multiple
*   types of data, multiple memory pools must be used.
*/
typedef struct s_Mempool
{
    // The head node of the linked list.
	OS_block_t* head;
    
    // Size in bytes of each OS_block_t.
	size_t blockSz;
    
    // The number of OS_block_t nodes in the linked list. Therefore, this field
    // also represents the number of data allocations available.
	size_t nBlocks;
    
    // Mutex lock to prevent simultaneous access.
    OS_mutex_t mux;
    
    // Counting semaphore to keep track of the number of allocations and 
    // deallocations to ensure the pool doesn't get emptied beyond its limits.
    OS_sem_t sem;
} OS_mempool_t;

/**
* @brief This function initialises a memory pool.
* @param pool The memory pool to initialise.
* @param blockSz The size, in bytes, of each block of memory in the pool. This 
*    size must be equal to size of the data type stored in the elements array.
* @param nBlocks The number of blocks in the memory pool. This must be equal
*   to the size of the elements array.
* @param elements Pointer to a statically declared array for which the memory
*   pool uses for allocations and deallocations.
*/
void OS_InitMempool(OS_mempool_t* const pool,
                      const size_t blockSz,
                      const size_t nBlocks,
                      void** elements);
                 
/**
* @brief This function allocates a block of memory in the memory pool and 
*   returns a pointer to it.
* @param pool Pointer to the pool to allocate memory from.                      
* @return Pointer to the allocated memory location.
*/                      
void* OS_Malloc(OS_mempool_t* const pool); 
                      
               
/**
* @brief Care must be taken not to call this function to many times as 
*   undefined behaviour will occur.                  
* @param pool Pointer to pool to deallocate memory from.
* @param item The data to put into the deallocated block.                      
*/                      
void OS_Dalloc(OS_mempool_t* const pool, void* const item); 

#define OS_MempoolAdd OS_Dalloc                  

#endif  // MEMORY_H
                      