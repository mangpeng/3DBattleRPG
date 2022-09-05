#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"

void ServerPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{
	default:
		break;
	}
}

SendBufferRef ServerPacketHandler::Make_STC_TEST(uint64 id, uint32 hp, uint16 attack, Xvector<BuffData> buffs)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id << hp << attack;

	// 가변 데이터
	bw << (uint16)buffs.size();

	for (auto& buff : buffs)
	{
		bw << buff.buffId << buff.remainTime;
	}

	header->size = bw.WriteSize();
	header->id = STC_TEST;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}
