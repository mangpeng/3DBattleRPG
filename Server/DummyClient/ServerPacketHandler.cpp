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
		// ĳ���� ������ ����â����
	}

	// ���� ����
	// ĳ���� �ӽ÷� ����
	Protocol::C_ENTER_GAME enterGamePkt;
	enterGamePkt.set_playerindex(0); 
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);

	session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	// �÷��̾� ���̵� ĳ��
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