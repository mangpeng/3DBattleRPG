#include "pch.h"
#include <iostream>

#include "ThreadManager.h"
#include "Memory.h"

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


#include "PlayerManager.h"
#include "AccountManager.h"
#include "Allocator.h"


class Knight
{
public:
	Knight() 
	{
		cout << "Knight()" << endl;
	}
	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	Knight(int hp) :_hp(hp) {}

public:
	int _hp;

};

int main()
{
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

