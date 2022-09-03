#include "pch.h"
#include <iostream>

#include "ThreadManager.h"
#include "Memory.h"

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


#include "Allocator.h"


class Knight
{
public:
	int _hp = rand() % 1000;
};

class Monster
{
public:
	int64 _id = 0;
};

int main()
{
	Knight* knights[100];
	for (int32 i = 0; i < 100; i++)
		knights[i] = ObjectPool<Knight>::Pop();

	for (int32 i = 0; i < 100; i++)
	{
		ObjectPool<Knight>::Push(knights[i]);
		knights[i] = nullptr;
	}

	shared_ptr<Knight> sptr = ObjectPool<Knight>::MakeShared();
	auto sptr2 = MakeShared<Knight>();


	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Xvector<Knight> v(10);

					Xmap<int32, Knight> m;
					m[100] = Knight();

					this_thread::sleep_for(10ms);
				}
			});
	}

	GThreadManager->Join();
}

