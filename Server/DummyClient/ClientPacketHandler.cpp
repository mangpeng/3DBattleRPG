#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

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

struct BuffData
{
	uint64 buffId;
	float remainTime;
};

struct STC_TEST
{
	uint64 id;
	uint32 hp;
	uint16 attack;

	// 가변 데이터
	Xvector<BuffData> buffs;
};

void ClientPacketHandler::Handle_STC_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	uint64 id;
	uint32 hp;
	uint16 attack;

	br >> id >> hp >> attack;

	Xvector<BuffData> buffs;
	uint16 buffCount;
	br >> buffCount;

	buffs.resize(buffCount);
	for(int32 i = 0; i < buffCount; i++)
	{
		br >> buffs[i].buffId >> buffs[i].remainTime;
	}

	cout << "ID : " << id << " HP : " << hp << " ATT : " << attack << endl;
	cout << "Buff Count : " << buffCount << endl;
	for (int32 i = 0; i < buffCount; i++)
	{
		cout << "BuffInfo : " << buffs[i].buffId << ", " << buffs[i].remainTime << endl;
	}

}
