#pragma once
#include "JobSerializer.h"

class Room : public JobSerializer
{
public:

	// �̱� ������ ó�� ���� => �ϰ��� ���ؼ��� ȣ�� �ǵ���
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:

	//void FlushJob(); // �ݵ�� �ϳ��� �����忡�� ȣ���ؾ� ��

	virtual void FlushJob() override;

private:
	//USE_LOCK; job queue ����ϹǷ� �ʿ� ����
	map<uint64, PlayerRef> _players;

	//JobQueue _jobs;
};


extern shared_ptr<Room> GRoom;
