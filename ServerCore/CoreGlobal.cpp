#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
#include "SocketUtils.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
Memory* GMemory = nullptr;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
	GDeadLockProfiler = new DeadLockProfiler();
	GMemory = new Memory();
	SocketUtils::init();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
	delete GDeadLockProfiler;
	delete GMemory;
	SocketUtils::clear();
}

