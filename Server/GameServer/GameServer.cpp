#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include "SocketUtils.h"

int main()
{

	SOCKET socket = SocketUtils::CreateSocket();

	SocketUtils::BindAnyAddress(socket, 7777);

	SocketUtils::Listen(socket);

	SOCKET clientSocket = ::accept(socket, nullptr, nullptr);

	cout << "Client Connected! " << endl;

	while (true)
	{

	}

	return 0;
}