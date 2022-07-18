#pragma once

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