#include "MemoryPool.h"
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "SLocker.h"
#include <iostream>

PMEMORYPOLL g_pmemory_poll = NULL;


// 创建chunk缓存
PMEMORYCHUNK create_chunk_cache(void *pbuffer, unsigned size)
{
	assert(pbuffer);
	// 第一个元素作为头节点
	PMEMORYCHUNK phead = (PMEMORYCHUNK)malloc(sizeof(MEMORYCHUNK));
	PMEMORYCHUNK pnode = (PMEMORYCHUNK)pbuffer;
	PMEMORYCHUNK _phead = phead;
	phead->pPrev = NULL;
	phead->pNext = NULL;
	for (unsigned i = 0; i < size; ++i)
	{
		PMEMORYCHUNK tmp = pnode + i;
		phead->pNext = tmp;
		tmp->pPrev = phead;
		tmp->pNext = NULL;
		phead = tmp;
	}
	return _phead;
}


// 创建chunk空闲内存缓存
PMEMORYCHUNKPOINT create_chunk_point_cache(void *pbuffer, unsigned size)
{
	assert(pbuffer);
	// 第一个元素作为头节点
	PMEMORYCHUNKPOINT phead = (PMEMORYCHUNKPOINT)malloc(sizeof(MEMORYCHUNKPOINT));
	PMEMORYCHUNKPOINT pnode = (PMEMORYCHUNKPOINT)pbuffer;
	PMEMORYCHUNKPOINT _phead = phead;
	phead->pPrev = NULL;
	phead->pNext = NULL;
	for (unsigned i = 0; i < size; ++i)
	{
		PMEMORYCHUNKPOINT tmp = pnode + i;
		phead->pNext = tmp;
		tmp->pPrev = phead;
		tmp->pNext = NULL;
		phead = tmp;
	}
	return _phead;
}


// 从缓存取出一个数据
PLISTNODE getchunk(PLISTNODE phead)
{
	assert(phead);
	if (phead == NULL || phead->pNext == NULL)
	{
		return NULL;
	}
	PLISTNODE pret = phead->pNext;
	phead->pNext = pret->pNext;
	return pret;
}

// 添加一个数据到缓存中
bool addchunkcache(PLISTNODE phead, PLISTNODE pnode)
{
	assert(phead && pnode);
	if (phead == NULL || pnode == NULL) {
		return false;
	}
	pnode->pNext = phead->pNext;
	if (pnode->pNext) {
		pnode->pNext->pPrev = pnode;
	}

	phead->pNext = pnode;
	pnode->pPrev = phead;
	return true;
}

PMEMORYCHUNK createList()
{
	PMEMORYCHUNK phead = (PMEMORYCHUNK)malloc(sizeof(MEMORYCHUNK));
	phead->pNext = NULL;
	phead->pPrev = NULL;
	phead->count = 0;
	phead->pmemory_block_head = NULL;
	phead->bUse = true;
	return phead;
}

PMEMORYCHUNKPOINT createChunkPointList()
{
	PMEMORYCHUNKPOINT phead = (PMEMORYCHUNKPOINT)malloc(sizeof(MEMORYCHUNKPOINT));
	phead->pNext = NULL;
	phead->pPrev = NULL;
	phead->pData = NULL;
	return phead;
}

bool pushFront(PLISTNODE head, PLISTNODE node)
{
	PLISTNODE pNext = head->pNext;
	head->pNext = node;
	node->pPrev = head;
	node->pNext = pNext;
	if (pNext)
	{
		pNext->pPrev = node;
	}
	
	return true;
}

bool insert_before(PLISTNODE head, PLISTNODE node, PLISTNODE insert_node)
{
	PLISTNODE prev = node->pPrev;
	insert_node->pPrev = prev;
	insert_node->pNext = node;
	node->pPrev = insert_node;
	prev->pNext = insert_node;
	
	return true;
}

bool list_delete(PLISTNODE head, PLISTNODE node)
{
	PLISTNODE prev = node->pPrev;

	prev->pNext = node->pNext;

	if (prev->pNext)
	{
		prev->pNext->pPrev = prev;
	}

	return true;
}

unsigned uptoblock(unsigned size)
{
	unsigned _size = size - size % BLOCKSIZE;
	if (_size == size)
	{
		return _size;
	}
	return _size + BLOCKSIZE;
}

void *index2addr(PMEMORYPOLL ppoll, unsigned index)
{
	return (char *)ppoll->pbuffer + index * BLOCKSIZE;
}

unsigned addr2index(PMEMORYPOLL ppoll, void *addr)
{
	return ((char *)addr - (char *)ppoll->pbuffer) / BLOCKSIZE;
}

// 创建内存池
PMEMORYPOLL create_memory_poll(unsigned size)
{
	assert(size >= NODESIZE);
	PMEMORYPOLL _memory_poll = (PMEMORYPOLL)malloc(size);

	if (_memory_poll == NULL) {
		return NULL;
	}

	memset(_memory_poll, 0, size);
	unsigned _size = size - sizeof(MEMORYPOLL); // 除去头的大小
	unsigned _diffsize = _size - _size % NODESIZE;
	_size = _diffsize;

	// 可以分配的block的数量
	_memory_poll->block_size = _size / NODESIZE;

	// 用于后面的内存对齐
	if (_diffsize < 8)
	{
		_memory_poll->block_size--;
	}

	if (_memory_poll->block_size == 0) { // 不够存放
		delete _memory_poll;
		return NULL;
	}

	// 已使用大小
	_memory_poll->used_size = 0;

	// block的开始段
	_memory_poll->pmemory_block_head = (PMEMORYBLOCK)((char *)_memory_poll + sizeof(MEMORYPOLL));

	// block占用的字节数
	unsigned block_buffer_size = sizeof(MEMORYBLOCK)* _memory_poll->block_size;

	// chunk缓存池
	_memory_poll->pmemory_chunk_list_cahce_poll = create_chunk_cache((char *)_memory_poll->pmemory_block_head + 
		block_buffer_size, _memory_poll->block_size);

	// 可以使用的内存偏移值
	unsigned data_buffer_offset = block_buffer_size + sizeof(MEMORYCHUNK)* _memory_poll->block_size;

	// 可使用内存
	_memory_poll->pbuffer = (char *)_memory_poll->pmemory_block_head + data_buffer_offset;

	// 地址对齐8字节
	unsigned long long ullBufAddr = (unsigned long long) _memory_poll->pbuffer;
	_memory_poll->pbuffer = (char *)_memory_poll->pbuffer + (8 - ullBufAddr % 8);

	// chunk链表
	_memory_poll->pmemory_chunk_list = createList();

	PMEMORYCHUNK pmemory_chunk = (PMEMORYCHUNK)getchunk((PLISTNODE)_memory_poll->pmemory_chunk_list_cahce_poll);


	pmemory_chunk->count = _memory_poll->block_size;
	pmemory_chunk->pmemory_block_head = _memory_poll->pmemory_block_head;
	pmemory_chunk->pPrev = NULL;
	pmemory_chunk->pNext = NULL;
	pmemory_chunk->bUse = false;
	pushFront((PLISTNODE)_memory_poll->pmemory_chunk_list, (PLISTNODE)pmemory_chunk);

	int free_max_size = _memory_poll->block_size / 2 + 1;

	char *buf = (char *)malloc(free_max_size * sizeof(MEMORYCHUNKPOINT));
	assert(buf);

	_memory_poll->_pfreechunkbuf = buf;
	_memory_poll->pmemory_chunk_free_list_cache = create_chunk_point_cache(buf, free_max_size);
	_memory_poll->pmemory_chunk_free_list = createChunkPointList();

	PMEMORYCHUNKPOINT pmemory_chunk_pt = (PMEMORYCHUNKPOINT)getchunk((PLISTNODE)_memory_poll->pmemory_chunk_free_list_cache);
	pmemory_chunk_pt->pData = pmemory_chunk;
	pmemory_chunk->pfree = pmemory_chunk_pt;

	pushFront((PLISTNODE)_memory_poll->pmemory_chunk_free_list, (PLISTNODE)pmemory_chunk_pt);
    
    _memory_poll->mutex = new std::mutex;
    

	return _memory_poll;
}
// 分配内存
void *poll_alloc(PMEMORYPOLL memory_poll, unsigned size)
{
	if (size <= 0) return NULL;
	SLocker locker(*(memory_poll->mutex));
	size = uptoblock(size);
	unsigned blocksize = size / BLOCKSIZE;
	PMEMORYCHUNKPOINT phead = memory_poll->pmemory_chunk_free_list;
	PMEMORYCHUNK pchunk = NULL;


	// 查找可以分配的内存块
	while (phead = phead->pNext)
	{
		if (phead->pData->count >= blocksize)
		{
			pchunk = phead->pData;
			break;
		}

	}
	if (pchunk == NULL) {
		assert(false);
		return NULL;
	}

	
	if (pchunk->count == blocksize) // 这个内存块使用完了
	{
		pchunk->bUse = true;
		unsigned index = pchunk->pmemory_block_head - memory_poll->pmemory_block_head;
		memory_poll->used_size += size;
		list_delete((PLISTNODE)memory_poll->pmemory_chunk_free_list, (PLISTNODE)phead);
		addchunkcache((PLISTNODE)memory_poll->pmemory_chunk_free_list_cache, (PLISTNODE)phead);
		return index2addr(memory_poll, index);
	}
	else {
		// 这个内存块没有使用完
		PMEMORYCHUNK pmemory_chunk = (PMEMORYCHUNK)getchunk((PLISTNODE)memory_poll->pmemory_chunk_list_cahce_poll);

		// 没有空闲内存块
		assert(pmemory_chunk);
		if (pmemory_chunk == NULL) return NULL;

		// 设置使用的节点
		pmemory_chunk->pmemory_block_head = pchunk->pmemory_block_head;
		pmemory_chunk->count = blocksize;
		pmemory_chunk->bUse = true;
		pmemory_chunk->pmemory_block_head->pmemory_chunk = pmemory_chunk;

		// 空闲内存属性重新设置
		pchunk->pmemory_block_head = pchunk->pmemory_block_head + blocksize;
		pchunk->count -= blocksize;
		pchunk->pmemory_block_head->pmemory_chunk = pchunk;
		insert_before((PLISTNODE)memory_poll->pmemory_chunk_list, (PLISTNODE)pchunk, (PLISTNODE)pmemory_chunk);
		unsigned index = pmemory_chunk->pmemory_block_head - memory_poll->pmemory_block_head;
		memory_poll->used_size += size;
		return index2addr(memory_poll, index);
	}
	assert(false);
	return NULL;
}
bool poll_free(PMEMORYPOLL memory_poll, void *addr)
{
	if (addr == NULL)
	{
		assert(false);
		return false;
	}
	SLocker locker(*(memory_poll->mutex));
	unsigned index = addr2index(memory_poll, addr);
	PMEMORYBLOCK pmemory_block = memory_poll->pmemory_block_head + index;
	PMEMORYCHUNK pmemory_chunk = pmemory_block->pmemory_chunk;
	if (pmemory_chunk->bUse == false) return false;

	PMEMORYCHUNK pprev_chunk = pmemory_chunk->pPrev;
	PMEMORYCHUNK pnext_chunk = pmemory_chunk->pNext;
	memory_poll->used_size -= pmemory_chunk->count * BLOCKSIZE;
    
    
	// 前后都不是空闲的
	if ((pprev_chunk == NULL || pprev_chunk->bUse == true)
		&& (pnext_chunk == NULL || pnext_chunk->bUse == true))
	{
		pmemory_chunk->bUse = false;
		PMEMORYCHUNKPOINT pmemory_chunk_pt = (PMEMORYCHUNKPOINT)getchunk((PLISTNODE)memory_poll->pmemory_chunk_free_list_cache);
		pmemory_chunk_pt->pData = pmemory_chunk;
		pmemory_chunk->pfree = pmemory_chunk_pt;


		pushFront((PLISTNODE)memory_poll->pmemory_chunk_free_list, (PLISTNODE)pmemory_chunk_pt);
		return true;
	}
    
	// 前一个内存块是空闲的
	if (pprev_chunk && pprev_chunk->bUse == false)
	{
		pprev_chunk->count += pmemory_chunk->count; // 和上一个节点合并
		list_delete((PLISTNODE)memory_poll->pmemory_chunk_list, (PLISTNODE)pmemory_chunk);  // 删除当前节点
		addchunkcache((PLISTNODE)memory_poll->pmemory_chunk_list_cahce_poll, (PLISTNODE)pmemory_chunk);  // 当前节点加入到缓存 
		pmemory_chunk->bUse = false;
		pmemory_chunk = pprev_chunk; // 设置当前节点为 上一个节点
	}

	// 下一个内存块是空闲的
	if (pnext_chunk && pnext_chunk->bUse == false)
	{
		pmemory_chunk->bUse = false;
		pmemory_chunk->count += pnext_chunk->count;
		pnext_chunk->bUse = false;
		list_delete((PLISTNODE)memory_poll->pmemory_chunk_list, (PLISTNODE)pnext_chunk);  // 删除下一个节点
		addchunkcache((PLISTNODE)memory_poll->pmemory_chunk_list_cahce_poll, (PLISTNODE)pnext_chunk);  // 下一个节点加入到缓存

		// 删除下一块内存块的空闲节点
		list_delete((PLISTNODE)memory_poll->pmemory_chunk_free_list, (PLISTNODE)(pnext_chunk->pfree));
		addchunkcache((PLISTNODE)memory_poll->pmemory_chunk_free_list_cache, (PLISTNODE)(pnext_chunk->pfree));

		// 没有合并上一个内存快
		if (pprev_chunk == NULL || (pprev_chunk && pprev_chunk->bUse == true))
		{
			// 添加新的空闲节点 指向现在的空闲内存
			PMEMORYCHUNKPOINT pmemory_chunk_pt = (PMEMORYCHUNKPOINT)getchunk((PLISTNODE)memory_poll->pmemory_chunk_free_list_cache);
			pmemory_chunk_pt->pData = pmemory_chunk;
			pmemory_chunk->pfree = pmemory_chunk_pt;

			pushFront((PLISTNODE)memory_poll->pmemory_chunk_free_list, (PLISTNODE)pmemory_chunk_pt);
		}
	}
	return true;
}


void destroy_memory_poll(PMEMORYPOLL memory_poll)
{
	if (NULL == memory_poll) return;
	free(memory_poll->pmemory_chunk_list);
	free(memory_poll->pmemory_chunk_list_cahce_poll);
	free(memory_poll->pmemory_chunk_free_list);
	free(memory_poll->pmemory_chunk_free_list_cache);
	free(memory_poll->_pfreechunkbuf);
    delete memory_poll->mutex;
	free(memory_poll);
}
