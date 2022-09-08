#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(JobRef job, bool pushOnly)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK

	// ó������ job�� ���� �����尡 ������� �Ѵ�.
	if (prevCount == 0)
	{
		// �������� jobqueue�� ������ ����
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue�� �ѱ��.
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		Xvector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute(); // ���⼭ Ÿ��Ÿ�� �ٸ� jobqueue�� �����ϰ� �Ǹ� �ش� jobqueue�� �������� ����.

		// ���� �ϰ��� 0����� ����
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		// �ϰ��� ��� ���ͼ� �ش� �����尡 ���� ���ϸ� ������ �ϰ��� global queue��
		// �� �ѱ��.
		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue�� �ѱ��.
			GGlobalQueue->Push(shared_from_this());
			break;
		}

	}
}
