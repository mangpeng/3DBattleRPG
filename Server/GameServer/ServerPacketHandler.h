#pragma once

enum
{
	STC_TEST = 1
};

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


class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

	static SendBufferRef Make_STC_TEST(uint64 id, uint32 hp, uint16 attack, Xvector<BuffData> buffs);
};

