#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 alloc_size) : alloc_size_(alloc_size)
{
	::InitializeSListHead(&header_); //초기화 작업
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&header_)))
	{
		::_aligned_free(memory);
	}
}

void MemoryPool::push(MemoryHeader* ptr)
{
	ptr->alloc_size = 0;
	::InterlockedPushEntrySList(&header_, static_cast<PSLIST_ENTRY>(ptr));

	alloc_count.fetch_sub(1);
}

MemoryHeader* MemoryPool::pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&header_));

	
	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(alloc_size_, SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(memory->alloc_size == 0);
	}

	alloc_count.fetch_add(1);

	return memory;
}
