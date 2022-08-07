#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

ThreadManager::ThreadManager()
{
	init_tls();
}

ThreadManager::~ThreadManager()
{
	join();
}

void ThreadManager::launch(std::function<void(void)> callback)
{
	LockGuard guard(lock_);

	threads_.push_back(thread([=]()
		{
			init_tls();
			callback();
			destroy_tls();
		}));
}

void ThreadManager::join()
{
	for (auto& t : threads_)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
	threads_.clear();
}

void ThreadManager::init_tls()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::destroy_tls()
{

}
