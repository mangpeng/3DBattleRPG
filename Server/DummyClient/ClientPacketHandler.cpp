#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

using namespace Protocol;

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case STC_TEST:
		Handle_STC_TEST(buffer, len);
		break;
	default:
		break;
	}
}


void ClientPacketHandler::Handle_STC_TEST(BYTE* buffer, int32 len)
{
	S_TEST pkt;

	ASSERT_CRASH(pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)));

	cout << pkt.id() << " " << pkt.hp() << " " << pkt.attack() << endl;

	cout << "buff count : " << pkt.buffs_size() << endl;

	for (auto& buf : pkt.buffs())
	{
		cout << "buf info :: " << buf.buffid() << " " << buf.remaintime() << endl;
		cout << "victims count : " << buf.victims_size() << endl;
		for (auto& victim : buf.victims())
		{
			cout << "victim info :: " << victim << endl;
		}
	}
}
