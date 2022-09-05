#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"

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
		BYTE* buffer = sendBuffer->Buffer();
		
		((PacketHeader*)buffer)->size = sizeof(sendData) + sizeof(PacketHeader);
		((PacketHeader*)buffer)->id = 1;

		::memcpy(sendBuffer->Buffer() + sizeof(PacketHeader), sendData, sizeof(sendData));
		sendBuffer->Close(sizeof(sendData) + sizeof(PacketHeader));

		GSessionManager.Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();

}