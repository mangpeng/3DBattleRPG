#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

shared_ptr<Room> GRoom = make_shared<Room>();

void Room::Enter(PlayerRef player)
{
	//WRITE_LOCK; job queue 사용하므로 필요 없음
	_players[player->playerId] = player;
}

void Room::Leave(PlayerRef player)
{
	//WRITE_LOCK; job queue 사용하므로 필요 없음
	_players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	//WRITE_LOCK; job queue 사용하므로 필요 없음
	for (auto& p : _players)
	{
		p.second->ownerSession->Send(sendBuffer);
	}
}

void Room::FlushJob()
{
	while (true)
	{
		JobRef job = _jobQueue.Pop();
		if (job == nullptr)
			break;

		job->Execute();
	}
}
