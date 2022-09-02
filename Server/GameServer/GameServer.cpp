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
	Knight() {}
	Knight(int hp) :_hp(hp) {}

public:
	int _hp;

};

int main()
{
	
}

