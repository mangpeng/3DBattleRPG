#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock()
{
	// ������ �����尡 �����ϰ� �ִٸ� ������ ����.
	const int lockThreadId = _lockFlag.load() & WRITE_THREAD_MASK >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	// �ƹ��� ���� �� �������� �ʰ� ���� �� �����ؼ� �������� ��´�.
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

void Lock::WriteUnlock()
{
	// ReadLock �� Ǯ�� ������ WriteUnlock �Ұ���
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock()
{
	// ������ �����尡 �����ϰ� �ִٸ� ������ ����
	const int lockThreadId = _lockFlag.load() & WRITE_THREAD_MASK >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// �ƹ��� ����(write ���°� �ƴ� ��)�ϰ� ���� ���� �� �����ؼ� ���� ī��Ʈ�� �ø���.
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 count = 0; count <= MAX_SPIN_COUNT; count++)
		{
			// write flag�� 0���� �и��ٴ� ���� write lock �������� �ʴ� �ٴ� ���
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

void Lock::ReadUnlock()
{
	// fetch_sub�� ���� ���� ��ȯ�Ѵ�.
	// ���� ���� 0 �̶�� ���� ���� �ִ� ��Ȳ
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE UNLOCK");
}
