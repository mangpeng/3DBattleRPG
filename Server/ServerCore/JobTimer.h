#pragma once

struct JobData
{
	JobData(weak_ptr<JobQueue> owner, JobRef job)
		: _owner(owner), _job(job)
	{

	}

	weak_ptr<JobQueue>	_owner;
	JobRef				_job;
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const 
	{
		return executeTick > other.executeTick;
	}

	uint64 executeTick = 0;
	JobData* jobData = nullptr; // priority queue에 들어가면서 정렬도면서 복사가 빈번하게 일어나므로 여기선 썡포인터를 드는 것이 효율적
};

class JobTimer
{
public:
	void		Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job);
	void		Distribute(uint64 now);
	void		Clear();

private:
	USE_LOCK;
	XpriorityQueue<TimerItem>		_items;
	Atomic<bool>					_distributing = false;

};

