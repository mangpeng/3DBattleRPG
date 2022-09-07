#pragma once
#include "Job.h"

class Room
{
	friend class EnterJob;
	friend class LeaveJob;
	friend class BroadcastJob;

private:

	// 싱글 스레드 처럼 동작 => 일감을 통해서만 호출 되도록
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:

	// 멀티스레드 환경에서는 일감으로 접근
	void PushJob(JobRef job) { _jobs.Push(job); } // jobqueue는 내부적으로 lock 들고 있음
	void FlushJob(); // 반드시 하나의 스레드에서 호출해야 함

private:
	//USE_LOCK; job queue 사용하므로 필요 없음
	map<uint64, PlayerRef> _players;

	JobQueue _jobs;
};


extern Room GRoom;

class EnterJob : public IJob
{
public:
	EnterJob(Room& room, PlayerRef player)
		:_room(room), _player(player)
	{

	}

	virtual void Execute() override
	{
		_room.Enter(_player);
	}

public:
	Room& _room;
	PlayerRef _player;
};

class LeaveJob : public IJob
{
public:
	LeaveJob(Room& room, PlayerRef player)
		:_room(room), _player(player)
	{

	}

	virtual void Execute() override
	{
		_room.Leave(_player);
	}

public:
	Room& _room;
	PlayerRef _player;
};

class BroadcastJob : public IJob
{
public:
	BroadcastJob(Room& room, SendBufferRef sendBuffer)
		:_room(room), _sendBuffer(sendBuffer)
	{

	}

	virtual void Execute() override
	{
		_room.Broadcast(_sendBuffer);
	}

public:
	Room& _room;
	SendBufferRef _sendBuffer;
};