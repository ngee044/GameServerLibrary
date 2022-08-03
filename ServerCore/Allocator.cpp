#include "pch.h"
#include "Allocator.h"


void* BaseAllocator::alloc(int32 size)
{
	return malloc(size);
}

void BaseAllocator::release(void* ptr)
{
	free(ptr);
}

void* StompAllocator::alloc(int32 size)
{
	const int64 page_count = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 data_offset = page_count * PAGE_SIZE - size;
	::VirtualAlloc(NULL, page_count + PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	void* base_address = ::VirtualAlloc(NULL, page_count * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	return static_cast<void*>(static_cast<int8*>(base_address) + data_offset);
}

void StompAllocator::release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 base_address = address - (address % PAGE_SIZE);

	::VirtualFree(reinterpret_cast<void*>(base_address), 0, MEM_RELEASE);
}
