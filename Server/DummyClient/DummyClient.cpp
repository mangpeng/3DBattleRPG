#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "ServerPacketHandler.h"

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
		Protocol::C_LOGIN pkt;
		
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
	}

	virtual void OnDisconnected() override
	{
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		ServerPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}
};

int main()
{
	ServerPacketHandler::Init();

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



	//while (true)
	//{
	//	if (GPlayerId == 0)
	//		continue;

	//	Protocol::C_CHAT chatPkt;
	//	chatPkt.set_playerid(GPlayerId);
	//	chatPkt.set_msg(u8"Hello, I'm client");
	//	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);

	//	service->BroadCast(sendBuffer);
	//	this_thread::sleep_for(1s);
	//}

	GThreadManager->Join();
}