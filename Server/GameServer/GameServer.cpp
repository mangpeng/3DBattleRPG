#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"

void HealByeValue(int64 target, int32 heal)
{
	cout << target << " " << heal << endl;
}

class Knight
{
public:
	void HealMe(int32 value)
	{
		cout << value << endl;
	}
};

int main()
{
	{
		FuncJob<void, int64, int32> job(HealByeValue, 100, 10);
		job.Execute();
	}

	{
		Knight k1;
		MemberJob job2(&k1, &Knight::HealMe, 10);
		job2.Execute();
	}
	


	ClientPacketHandler::Init();

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

	// 메인 스레드가 room의 일감 처리
	while (true)
	{
		GRoom.FlushJob();
		this_thread::sleep_for(1ms);
	}

	GThreadManager->Join();
}