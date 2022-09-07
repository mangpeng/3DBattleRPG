#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	static Atomic<uint16> idGenerator = 1;

	{
		// 임시 캐릭터 정보 생성
		auto player = loginPkt.add_players();
		player->set_name(u8"test id");
		player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);

		// 메모리에 캐릭터 정보 캐싱
		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->_players.push_back(playerRef);
	}


	{
		// 임시 캐릭터 정보 생성
		auto player = loginPkt.add_players();
		player->set_name(u8"test id2");
		player->set_playertype(Protocol::PLAYER_TYPE_MAGE);

		// 메모리에 캐릭터 정보 캐싱
		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->_players.push_back(playerRef);
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	uint64 index = pkt.playerindex();

	PlayerRef  player = gameSession->_players[index];
	GRoom.PushJob(MakeShared<EnterJob>(GRoom, player)); // 일감을 바로 실행하지 않고 예약
	//GRoom.Enter(player); // WRITE_LOCK  job 방식으로 변경

	Protocol::S_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
	player->ownerSession->Send(sendBuffer);


	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	cout << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	GRoom.PushJob(MakeShared<BroadcastJob>(GRoom, sendBuffer));
	//GRoom.Broadcast(sendBuffer); // WRITE_LOCK

	return true;
}
