#include <iostream>
#include "pch.h"

#include <thread>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	this_thread::sleep_for(1s);

	// 윈속 초기화(ws2_32 라이브러리 초기화)
	// 관련 정보가 wsaData에 채워짐
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;


	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	// 논블로킹 소켓으로 지정
	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	this_thread::sleep_for(1s);

	//Connect
	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			// 데이터가 아직 안 왔을 뿐
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			// 이미 연결 되어 있는 상태
			if (::WSAGetLastError() == WSAEISCONN)
				break;

			//error
			break;
		}
	}

	cout << "Connedted to Server!" << endl;

	//Send
	char sendBuffer[100] = "Hello World";
	WSAEVENT wsaEvent = ::WSACreateEvent();
	WSAOVERLAPPED overlapped = {};
	overlapped.hEvent = wsaEvent;

	while (true)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = sendBuffer;
		wsaBuf.len = 100;

		DWORD sendLen = 0;
		DWORD flags = 0;

		if (::WSASend(clientSocket, &wsaBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				// Pending
				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
				::WSAGetOverlappedResult(clientSocket, &overlapped, &sendLen, FALSE, &flags);
			}
			else
			{
				cout << "Error : " << ::WSAGetLastError() << endl;
				break;
			}
		}

		cout << "Send Data Len = " << sizeof(sendBuffer) << endl;

		this_thread::sleep_for(1s);

	}

	//Send

	// OS에게 소켓 리소스 반환
	::closesocket(clientSocket);

	// 윈속 종료
	::WSACleanup();

	return 0;
}