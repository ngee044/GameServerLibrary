#pragma once
#include "Types.h"
#include "Memory.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* pop(Args&&... args);
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::alloc(alloc_size_));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, alloc_size_));
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(pool_.pop(), alloc_size_));
#endif
		
		new(memory)Type(std::forward<Args>(args)...); //placement new
		return memory;
	}

	static void push(Type* obj) 
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::release(MemoryHeader::DetachHeader(obj));
#else
		pool_.push(MemoryHeader::DetachHeader(obj));
#endif
	}

	template<typename... Args>
	static shared_ptr<Type> xmake_shared(Args&... args)
	{
		std::shared_ptr<Type> ptr = { pop(std::forward<Arg>(args)...), push };
		return ptr;
	}

private:
	static int32 alloc_size_;
	static MemoryPool pool_;
};

template<typename Type>
int32 ObjectPool<Type>::alloc_size_ = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::pool_{ alloc_size_ };

