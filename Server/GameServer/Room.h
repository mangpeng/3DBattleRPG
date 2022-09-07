#pragma once
#include "JobSerializer.h"

class Room : public JobSerializer
{
public:

	// 싱글 스레드 처럼 동작 => 일감을 통해서만 호출 되도록
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:

	//void FlushJob(); // 반드시 하나의 스레드에서 호출해야 함

	virtual void FlushJob() override;

private:
	//USE_LOCK; job queue 사용하므로 필요 없음
	map<uint64, PlayerRef> _players;

	//JobQueue _jobs;
};


extern shared_ptr<Room> GRoom;
