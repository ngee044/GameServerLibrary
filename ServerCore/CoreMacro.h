#pragma once
#include "RWLock.h"

#define OUT

//Lock

#define USE_MANY_LOCKS(count) RWLock locks_[count];
#define USE_LOCK			  USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)	  ReadLockGuard read_lock_guard_##idx(locks_[idx], typeid(this).name());
#define READ_LOCK			  READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)   WriteLockGuard write_lock_guard_##idx(locks_[idx], typeid(this).name());
#define WRITE_LOCK			  WRITE_LOCK_IDX(0)

//Crash
#define CRASH(cause)						\
{											\
uint32* crash = nullptr;					\
__analysis_assume(crash != nullptr);		\
*crash = 0xDEADBEEF;						\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
									\
}									\

