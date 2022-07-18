#include "pch.h"
#include "RWLock.h"
#include "CoreTLS.h"


void RWLock::WriteLock()
{
	//동일한 쓰레드가 소유하고 있다면 무조건 성공!
	const uint32 lock_thread_id = (lock_flag_.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lock_thread_id)
	{
		write_count_++;
		return;
	}

	//아무도 소유 및 공유하고 있지 않을 때, 경합해서 소유권을 얻는다.
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);

	while (true)
	{
		for (uint32 spin_count = 0; spin_count < MAX_SPIN_COUNT; spin_count++)
		{
			uint32 expected = EMPTY_FLAG;
			if (lock_flag_.compare_exchange_strong(OUT expected, desired))
			{
				write_count_++;
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK_TIMEOUT");
		}

		this_thread::yield();
	}
}

void RWLock::WriteUnlock()
{
	// ReadLock 다 풀기 전에는 WriteUnlock 불가능
	if (lock_flag_.load() & READ_COUNT_MASK != 0)
	{
		CRASH("INVALID_UNLOCK_ORDER");
	}

	const int32 lock_count = --write_count_;
	if (lock_count == 0)
	{
		lock_flag_.store(EMPTY_FLAG);
	}
}

void RWLock::ReadLock()
{
	//동일한 쓰레드가 소유하고 있다면 무조건 성공
	//아무도 소유하고 있지 않을 때 경합해서 공유 카운트를 올린다.
	const uint32 lock_thread_id = (lock_flag_.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lock_thread_id)
	{
		lock_flag_.fetch_add(1);
		write_count_++;
		return;
	}
	
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spin_count = 0; spin_count < MAX_SPIN_COUNT; spin_count++)
		{
			uint32 expected = (lock_flag_.load() & READ_COUNT_MASK);
			if (lock_flag_.compare_exchange_strong(OUT expected, expected + 1))
			{
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK_TIMEOUT");
		}

		this_thread::yield();

	}
}

void RWLock::ReadUnlock()
{
	if ((lock_flag_.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{

	}
}


