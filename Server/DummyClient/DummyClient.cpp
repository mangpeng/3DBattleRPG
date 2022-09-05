#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

BYTE sendData[] = "Hello World";

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		//cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		//cout << "Connected to Server" << endl;

		//SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
		//::memcpy(sendBuffer->Buffer(), sendData, ulen32(sendData));
		//sendBuffer->Close(ulen32(sendData));

		//Send(sendBuffer);
	}

	virtual void OnDisconnected() override
	{
	}

	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketHeader header = *(reinterpret_cast<PacketHeader*>(buffer));

		BYTE recvBuffer[4096];
		::memcpy(recvBuffer, buffer + sizeof(PacketHeader), header.size - sizeof(PacketHeader));
		cout << recvBuffer << endl;

		return len;
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, // TODO : SessionManager에서 관리 필요
		1000);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}