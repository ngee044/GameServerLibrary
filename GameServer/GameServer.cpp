#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>

#include "pch.h"
#include "CoreMacro.h"
#include "ThreadManager.h"


class TestLock
{
	USE_LOCK;

public:
	int32 TestRead()
	{
		READ_LOCK;

		if (queue_.empty())
			return -1;

		return queue_.front();
	}

	void TestPush()
	{
		WRITE_LOCK;

		queue_.push(rand() % 100);
	}

	void TestPop()
	{
		WRITE_LOCK;

		if (queue_.empty() == false)
		{
			queue_.pop();
		}
	}

private:
	std::queue<int32> queue_;
};


void ThreadMain()
{
	while (true)
	{
		std::cout << "Hello ! I am thread " << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

TestLock testLock;

void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		std::this_thread::sleep_for(1ms);
		testLock.TestPop();

	}
}

void ThreadRead()
{
	while(true)
	{
		int32 value = testLock.TestRead();
		std::cout << value << std::endl;
		std::this_thread::sleep_for(1ms);
	}
}

CoreGlobal Core;

int main()
{
#if 0
	SYSTEM_INFO info;
	::GetSystemInfo(&info);

	std::cout << info.dwPageSize << std::endl;
	std::cout << info.dwAllocationGranularity << std::endl;

	int* test = (int*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	*test = 100;
	::VirtualFree(test, 0, MEM_RELEASE);

	for (auto i = 0; i < 2; ++i)
	{
		GThreadManager->Launch(ThreadWrite);
	}

	for (auto i = 0; i < 5; ++i)
	{
		GThreadManager->Launch(ThreadRead);
	}
#endif

	GThreadManager->Join();
	std::cout << __func__ << "()" << std::endl;
	return 0;
} 