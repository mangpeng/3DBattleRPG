#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

class ClientSession : public Session 
{
public:
	~ClientSession()
	{
		cout << "~ClientSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected to Server" << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected from Server" << endl;
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "OnRecv Len = " << len << endl;
		Send(buffer, len);
		return len;
	}

	virtual void OnSend(int32 len) override
	{
		cout << "OnSend Len = " << len << endl;
	}
};

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>, // TODO : SessionManager에서 관리 필요
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

	GThreadManager->Join();

}