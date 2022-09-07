#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:

	// �̱� ������ ó�� ���� => �ϰ��� ���ؼ��� ȣ�� �ǵ���
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

private:
	//USE_LOCK; job queue ����ϹǷ� �ʿ� ����
	map<uint64, PlayerRef> _players;
};


extern shared_ptr<Room> GRoom;
