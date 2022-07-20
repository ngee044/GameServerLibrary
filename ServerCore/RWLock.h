#pragma once
#include "Types.h"

/*
[WWWW WWWW][WWWW WWWW][RRRR RRRR][RRRR RRRR]
W: WriteFlag (Exclusive Lock Owner ThreadID)
R: ReadFlag (Shared Lock Count)
*/

//Read Write Lock

class RWLock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32> lock_flag_ = EMPTY_FLAG;
	uint16 write_count_ = 0;
};


class ReadLockGuard
{
public:
	ReadLockGuard(RWLock& rw_lock, const char* name) : rw_lock_(rw_lock), name_(name) { rw_lock_.ReadLock(name); }
	~ReadLockGuard() { rw_lock_.ReadUnlock(name_); }
private:
	RWLock& rw_lock_;
	const char* name_;
};


class WriteLockGuard
{
public:
	WriteLockGuard(RWLock& rw_lock, const char* name) : rw_lock_(rw_lock), name_(name) { rw_lock_.WriteLock(name); }
	~WriteLockGuard() { rw_lock_.WriteUnlock(name_); }
private:
	RWLock& rw_lock_;
	const char* name_;
};
