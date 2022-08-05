#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 alloc_size) : alloc_size_(alloc_size)
{
}

MemoryPool::~MemoryPool()
{
	while (!queue_.empty())
	{
		MemoryHeader* header = queue_.front();
		queue_.pop();
		::free(header);
	}
}

void MemoryPool::push(MemoryHeader* ptr)
{
}

MemoryHeader* MemoryPool::pop()
{
	MemoryHeader* header = nullptr;
	{
		WRITE_LOCK;

		if (!queue_.empty())
		{
			header = queue_.front();
			queue_.pop();
		}
	}

	if (header == nullptr)
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(alloc_size_));
	}
	else
	{
		ASSERT_CRASH(header->alloc_size == 0);
	}

	alloc_count.fetch_add(1);

	return header;
}
