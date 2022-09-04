#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

BYTE sendData[] = "Hello World";

class ServerSession : public Session
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected to Server" << endl;

		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
		sendBuffer->CopyData(sendData, sizeof(sendData));
		Send(sendBuffer);
	}

	virtual void OnDisconnected() override
	{
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "OnRecv Len = " << len << endl;

		this_thread::sleep_for(1s);

		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
		sendBuffer->CopyData(sendData, sizeof(sendData));
		Send(sendBuffer);

		return len;
	}

	virtual void OnSend(int32 len) override
	{
		cout << "OnSend Len = " << len << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, // TODO : SessionManager에서 관리 필요
		5);

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