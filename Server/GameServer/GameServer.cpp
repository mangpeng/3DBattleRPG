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
	WORKER_TICK = 64, // ������ �ϰ� ó������ ���� �ڵ� ���� �ǵ��� ���� �ʿ�
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		//  �����带 �����ϰ� ��ġ�ϴ� ���� �߿��ϴ�.
		// �Ʒ��� ���� ���� �����Ǻ��� �����带 ��ġ���� �ʰ�
		// ��� �����尡 �����ϲ�ó�� ��ġ �Ѵ�.
		// Iocp ������� 10��ŭ ��ٸ��� Ÿ�Ӿƿ��ȴٸ� DoGlobalQueueWork�� �����Ѵ�.
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// ��Ʈ��ũ ����� ó�� �����尡 -> �ΰ��� ��������(��Ŷ �ڵ鷯�� ����) ȣ�� ��
		service->GetIocpCore()->Dispatch(10);

		// ����� �ϰ� ó��(�ѹ��� �� �����常 ó�� ����)
		ThreadManager::DistributeReservedJobs();

		// �۷ι� ť
		ThreadManager::DoGlobalQueueWork();

	}
}

int main()
{
	//GRoom->DoTimer(1000, [] {cout << "Hello 1000" << endl; });
	//GRoom->DoTimer(2000, [] {cout << "Hello 2000" << endl; });
	//GRoom->DoTimer(3000, [] {cout << "Hello 3000" << endl; });

	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager���� ���� �ʿ�
		50);

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