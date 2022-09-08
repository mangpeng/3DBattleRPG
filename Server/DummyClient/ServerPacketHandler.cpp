#include "pch.h"
#include "ServerPacketHandler.h"
#include <iomanip>

PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);


	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == false)
		return true;

	if (pkt.players().size() == 0)
	{
		// 캐릭터 없으면 생성창으로
	}

	// 게임 입장
	// 캐릭터 임시로 선택
	Protocol::C_ENTER_GAME enterGamePkt;
	enterGamePkt.set_playerindex(0); 
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);

	session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	// 플레이어 아이디 캐싱
	GPlayerId = pkt.playerid();

	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	cout << "Recv : " << endl;
	cout << "[" << setw(4) << setfill('0') << pkt.playerid();
	cout << "] " << pkt.msg() << endl;

	//if (pkt.playerid() == GPlayerId)
	//{
	//	cout << "[" << setw(4) << "self";
	//	cout << "] " << pkt.msg() << endl;
	//}
	//else
	//{
	//	cout << "[" << setw(4) << setfill('0') << pkt.playerid();
	//	cout << "] " << pkt.msg() << endl;
	//}

	return true;
}