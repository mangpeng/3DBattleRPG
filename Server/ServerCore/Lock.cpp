#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	// 동일한 쓰레드가 소유하고 있다면 무조건 성공.
	const uint32 a = _lockFlag.load();
	const uint32 b = _lockFlag.load() & WRITE_THREAD_MASK;
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	// 아무도 소유 및 공유하지 않고 있을 떄 경합해서 소유권을 얻는다.
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 count = 0; count <= MAX_SPIN_COUNT; count++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				_writeCount++;
				return;
			}
		}

		uint64 elapsedTime = GetTickCount64() - beginTick;
		if (elapsedTime >= ACQUIRE_TIMEOUT_TICK)
				CRASH("Lock Time out");

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	// ReadLock 다 풀기 전에는 WriteUnlock 불가능
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공
	const int lockThreadId = _lockFlag.load() & WRITE_THREAD_MASK >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// 아무도 소유(write 상태가 아닐 떄)하고 있지 않을 때 경합해서 공유 카운트를 올린다.
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 count = 0; count <= MAX_SPIN_COUNT; count++)
		{
			// write flag가 0으로 밀린다는 것은 write lock 존재하지 않는 다는 얘기
			uint32 expected = _lockFlag.load() & READ_COUNT_MASK;
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		uint64 elapsedTime = GetTickCount64() - beginTick;
		if (elapsedTime >= ACQUIRE_TIMEOUT_TICK)
			CRASH("Lock Time out");

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	// fetch_sub는 이전 값읇 반환한다.
	// 이전 값이 0 이라는 것은 문제 있는 상황
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE UNLOCK");
}
