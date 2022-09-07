#pragma once
#include "Job.h"

class Room
{
public:

	// 싱글 스레드 처럼 동작 => 일감을 통해서만 호출 되도록
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:

	// 멀티스레드 환경에서는 일감으로 접근
	void PushJob(JobRef job) { _jobs.Push(job); } // jobqueue는 내부적으로 lock 들고 있음
	
	
	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::* memFunc)(Args...), Args... args)
	{
		auto job = MakeShared<MemberJob<T, Ret, Args... >> (static_cast<T*>(this), memFunc, args...);
		_jobs.Push(job);
	}

	// TODO:  static PushJob함수는 별도로 만들어야 함

	void FlushJob(); // 반드시 하나의 스레드에서 호출해야 함

private:
	//USE_LOCK; job queue 사용하므로 필요 없음
	map<uint64, PlayerRef> _players;

	JobQueue _jobs;
};


extern Room GRoom;
