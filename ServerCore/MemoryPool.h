#pragma once

struct MemoryHeader
{

	MemoryHeader(int32 size) : alloc_size(size) { }

	static void* attach_header(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 alloc_size;
};

class MemoryPool
{
public:
	MemoryPool(int32 alloc_size);
	~MemoryPool();

	void push(MemoryHeader* ptr);
	MemoryHeader* pop();


private:
	int32 alloc_size_ = 0;
	std::atomic<int32> alloc_count = 0;

	USE_LOCK;
	std::queue<MemoryHeader*> queue_;
};

