#pragma once

class BaseAllocator
{
public:
	static void* alloc(int32 size);
	static void release(void* ptr);
};

/*
stmp allocator
*/

class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };

public:
	static void* alloc(int32 size);
	static void release(void* ptr);
};

/*
pool allocator
*/

class PoolAllocator
{
public:
	static void* alloc(int32 size);
	static void release(void* ptr);
};

/*
STL Allocator
*/

template<typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() { }

	template<typename other>
	StlAllocator(const StlAllocator<other>&) { }

	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(PoolAllocator::alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::release(ptr);
	}


};
