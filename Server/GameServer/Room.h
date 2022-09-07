#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:

	// 싱글 스레드 처럼 동작 => 일감을 통해서만 호출 되도록
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

private:
	//USE_LOCK; job queue 사용하므로 필요 없음
	map<uint64, PlayerRef> _players;
};


extern shared_ptr<Room> GRoom;
