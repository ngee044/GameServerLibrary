#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

/*
	Memory
*/

Memory::Memory()
{
	int32 size = 0;
	int32 table_index = 0;

	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		pools_.push_back(pool);

		while (table_index <= size)
		{
			pool_table_[table_index] = pool;
			table_index++;
		}
	}

	for (; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		pools_.push_back(pool);

		while (table_index <= size)
		{
			pool_table_[table_index] = pool;
			table_index++;
		}
	}

	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		pools_.push_back(pool);

		while (table_index <= size)
		{
			pool_table_[table_index] = pool;
			table_index++;
		}
	}
}

Memory::~Memory()
{
	for (MemoryPool* pool : pools_)
		delete pool;

	pools_.clear();
}

void* Memory::allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 alloc_size = size + sizeof(MemoryHeader);

	if (alloc_size > MAX_ALLOC_SIZE)
	{
		//메모리 풀링 최대 크기를 벗어나면 일반 할당
		header = reinterpret_cast<MemoryHeader*>(::malloc(alloc_size));
	}
	else
	{
		//메모리 풀에서 꺼내온다
		header = pool_table_[alloc_size]->pop();
	}

	return MemoryHeader::attach_header(header, alloc_size);
}

void Memory::release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int32 alloc_size = header->alloc_size;
	ASSERT_CRASH(alloc_size > 0);

	if (alloc_size > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 해제
		::free(header);
	}
	else
	{
		// 메모리 풀에 반납한다.
		pool_table_[alloc_size]->push(header);
	}
}

