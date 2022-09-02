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


class Knight
{
public:
	Knight() {}
	Knight(int hp) :_hp(hp) {}

private:
	int _hp;

};

int main()
{
	Knight* knight = Xnew<Knight>(100);

	Xdelete(knight);
}

