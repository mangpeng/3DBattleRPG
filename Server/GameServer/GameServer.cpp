#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager에서 관리 필요
		100);


	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while(true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	BYTE sendData[1000] = "HelloWorld";

	while (true)
	{
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
		
		BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

		PacketHeader* header = bw.Reserve<PacketHeader>();

		// uint64 : id, uint32 : hp, uint16 :: attack
		bw << (uint64)1001 << (uint32)100 << (uint16)10;

		// 가변 데이터
		bw.Write(sendData, sizeof(sendData));

		header->size = bw.WriteSize();
		header->id = 1;

		sendBuffer->Close(bw.WriteSize());

		GSessionManager.Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();

}