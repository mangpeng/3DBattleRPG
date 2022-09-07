#pragma once
#include "Job.h"

class Room
{
public:

	// �̱� ������ ó�� ���� => �ϰ��� ���ؼ��� ȣ�� �ǵ���
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:

	// ��Ƽ������ ȯ�濡���� �ϰ����� ����
	void PushJob(JobRef job) { _jobs.Push(job); } // jobqueue�� ���������� lock ��� ����
	
	
	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::* memFunc)(Args...), Args... args)
	{
		auto job = MakeShared<MemberJob<T, Ret, Args... >> (static_cast<T*>(this), memFunc, args...);
		_jobs.Push(job);
	}

	// TODO:  static PushJob�Լ��� ������ ������ ��

	void FlushJob(); // �ݵ�� �ϳ��� �����忡�� ȣ���ؾ� ��

private:
	//USE_LOCK; job queue ����ϹǷ� �ʿ� ����
	map<uint64, PlayerRef> _players;

	JobQueue _jobs;
};


extern Room GRoom;
