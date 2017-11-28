//////////////////////////////////////
#ifndef MEMORY_POLL_HEAD
#define MEMORY_POLL_HEAD

#define BLOCKSIZE 64
#define ALIGN_ADDR_LEN 8
#define ALIGN_ADDRESS(addr) (addr = (char *)(addr) - ((int)(addr) % ALIGN_ADDR_LEN))
#define ALIGN_BLOCK(size) (size = (size) - (size) % BLOCKSIZE)
#define NODESIZE (BLOCKSIZE + sizeof(MemoryChunk) + sizeof(MemoryBlock))
#define INIT_MEMPORY_POLL(size) (g_pmemory_poll = create_memory_poll(size))
#define POLL_ALLOC(size) poll_alloc(g_pmemory_poll, (size))
#define POLL_FREE(addr) poll_free(g_pmemory_poll, (addr))
#define DESTROY_MEMORY_POLL() destroy_memory_poll(g_pmemory_poll)

#include <mutex>

struct MemoryChunk;

// 一个内存节点(小块)
typedef struct MemoryBlock
{
	MemoryChunk *pmemory_chunk; // 属于哪个内存块
}MEMORYBLOCK, *PMEMORYBLOCK;


// 分配的内存块结构
typedef struct ListNode // 分配的内存块
{
	struct ListNode *pPrev; // 上一个内存块
	struct ListNode *pNext; // 下一个内存块
}LISTNODE, *PLISTNODE;

// 分配的内存块结构
typedef struct MemoryChunkPoint // 分配的内存块
{
	struct MemoryChunkPoint *pPrev; // 上一个内存块
	struct MemoryChunkPoint *pNext; // 下一个内存块
	struct MemoryChunk *pData; // chunk指针
}MEMORYCHUNKPOINT, *PMEMORYCHUNKPOINT;

// 分配的内存块结构
typedef struct MemoryChunk // 分配的内存块
{
	struct MemoryChunk *pPrev; // 上一个内存块
	struct MemoryChunk *pNext; // 下一个内存块
	PMEMORYCHUNKPOINT pfree; // 索引空闲列表
	unsigned count; // 节点的数量
	struct MemoryBlock *pmemory_block_head; // 内存节点头
	bool bUse;
}MEMORYCHUNK, *PMEMORYCHUNK;




// 内存池
typedef struct MemoryPoll
{
	PMEMORYCHUNK pmemory_chunk_list_cahce_poll;	// 内存块链表节点
	PMEMORYCHUNK pmemory_chunk_list;	// 空闲块链表
	PMEMORYCHUNKPOINT pmemory_chunk_free_list_cache;	// 空闲块链表
	PMEMORYCHUNKPOINT pmemory_chunk_free_list;	// 空闲块链表
	PMEMORYBLOCK pmemory_block_head;		// block头
	void *pbuffer; // 分配的内存开始
	unsigned block_size; // 内存块数量
	unsigned used_size;  // 已经使用的大小
	void *_pfreechunkbuf;
    std::mutex *mutex;
}MEMORYPOLL, *PMEMORYPOLL;

extern PMEMORYPOLL g_pmemory_poll;

// 创建内存池
PMEMORYPOLL create_memory_poll(unsigned size);

// 分配内存
void *poll_alloc(PMEMORYPOLL, unsigned size);

// 释放内存
bool poll_free(PMEMORYPOLL memory_poll, void *addr);

// 销毁内存池
void destroy_memory_poll(PMEMORYPOLL memory_poll);

#endif // MEMORY_POLL_HEAD
