#pragma once
#include "Allocator.h"

class MemoryPool;

class Memory
{
	enum
	{
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	void* allocate(int32 size);
	void release(void* ptr);

private:
	std::vector<MemoryPool*> pools_;
	MemoryPool* pool_table_[MAX_ALLOC_SIZE + 1];

};


template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::alloc(sizeof(Type)));
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	PoolAllocator::release(obj);
}

template<typename Type>
std::shared_ptr<Type> xmake_shared()
{
	return shared_ptr<Type>{ xnew<Type>(), xdelete<Type> };
}