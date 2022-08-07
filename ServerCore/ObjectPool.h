#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* pop(Args&&... args);
	{
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(pool_.pop(), alloc_size_));
		new(memory)Type(std::forward<Args>(args)...); //placement new
		return memory;
	}

	static void push(Type* obj) 
	{
		obj->~Type();
		pool_.push(MemoryHeader::DetachHeader(obj));
	}

	static shared_ptr<Type> make_shared()
	{
		std::shared_ptr<Type> ptr = { Pop(), Push };
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

