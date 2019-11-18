#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__

#include <stdlib.h>


namespace Base {


#define MINUNITSIZE     64
#define ADDR_ALIGN      8
#define SIZE_ALIGN      MINUNITSIZE

struct memory_chunk;

typedef struct memory_block {
    size_t count;
    size_t start;
    memory_chunk* pmem_chunk;
} memory_block;


typedef struct memory_chunk {
    memory_block* pfree_mem_addr;
    memory_chunk* pre;
    memory_chunk* next;
} memory_chunk;


typedef struct MEMORYPOOL {
    void*           memory;
    size_t          size;
    memory_block*   pmem_map;
    memory_chunk*   pfree_mem_chunk;
    memory_chunk*   pfree_mem_chunk_pool;
    size_t          mem_used_size; // 记录内存池中已经分配给用户的内存的大小  
    size_t          mem_map_pool_count; // 记录链表单元缓冲池中剩余的单元的个数，个数为0时不能分配单元给pfree_mem_chunk  
    size_t          free_mem_chunk_count; // 记录 pfree_mem_chunk链表中的单元个数  
    size_t          mem_map_unit_count; //   
    size_t          mem_block_count; // 一个 mem_unit 大小为 MINUNITSIZE  
} MEMORYPOOL, *PMEMORYPOOL;


PMEMORYPOOL CreateMemoryPool(void* pBuf, size_t sBufSize);


void ReleaseMemoryPool(PMEMORYPOOL* ppMem);


void* GetMemory(size_t sMemorySize, PMEMORYPOOL pMem);


void FreeMemory(void *ptrMemoryBlock, PMEMORYPOOL pMem);

} // end namespace Base

#endif


