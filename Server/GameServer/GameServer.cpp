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

enum
{
	WORKER_TICK = 64, // 스레드 일감 처리률에 따라 자동 보정 되도록 수정 필요
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		//  스레드를 적절하게 배치하는 것은 중요하다.
		// 아래와 같은 경우는 포지션별로 스레드를 배치하지 않고
		// 모든 스레드가 만능일꾼처럼 배치 한다.
		// Iocp 입출력이 10만큼 기다리고 타임아웃된다면 DoGlobalQueueWork를 실행한다.
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 스레드가 -> 인게임 로직까지(패킷 핸들러에 의해) 호출 중
		service->GetIocpCore()->Dispatch(10);

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();

	}
}

int main()
{
	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager에서 관리 필요
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// MainThread
	DoWorkerJob(service);

	GThreadManager->Join();
}