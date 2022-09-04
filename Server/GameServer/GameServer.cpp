#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include "SocketUtils.h"
#include "ThreadManager.h"
#include "Listener.h"

#include "Service.h"
#include "Session.h"

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<Session>, // TODO : SessionManager에서 관리 필요
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

	return 0;
}