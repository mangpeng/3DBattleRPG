#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Send(BYTE* buffer, int32 len)
{
	// 문제 : recv와 다르게 send는 언제 어디서(멀티스레드)서 호출된다.
	// 1. 버퍼 관리는 어떻게 할지?
	// 2. sendEvent 관리 ? 단일? 여러개? WSASend 중첩?

	// TEMP
	SendEvent* sendEvent = Xnew<SendEvent>();
	sendEvent->owner = shared_from_this(); // add ref
	sendEvent->buffer.resize(len);
	::memcpy(sendEvent->buffer.data(), buffer, len);


	WRITE_LOCK; 
	// WSASend함수가 thread-unsafe 하다.
	// WSASend는 thread-unsafe 하기 떄문에 관리필요.
	// 1.
	// WSASend까지는 순서 보장이 되나(pending 되거나 하면서)
	// 워커 스레드에 의해 GetQueuedCompletionStatus 되는 시점은 순서를 보장할 수없다.
	// 2.
	// pending이 뜬다는건 이미 커널 sendbuffer가 꽉 찼다는 것을 의미하는데
	// 이떄 계속해서 send하는 것이 옳은 일인가?
	// 3.
	// scatter->gather를 이용하여 wsabuf를 모아서 보내는 것이 효과적이다.
	RegisterSend(sendEvent);
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	wcout << "Disconnect : " << cause << endl;

	OnDisconnected(); // 컨텐츠 코드에서 오버라이딩
	GetService()->ReleaseSession(GetSessionRef());

	RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, /*남은거*/ 0) == false)
		return false;


	_connectEvent.Init();
	_connectEvent.owner = shared_from_this(); // add ref

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		int32 erroCode = ::WSAGetLastError();
		if (erroCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr; // sub ref;
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this();

	// TF_REUSE_SOCKET: 사용시 AcceptEx, ConnectEx시 해당 소켓을 재사용할 수 있다.
	// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms737757(v=vs.85)
	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 erroCode = ::WSAGetLastError();
		if (erroCode != WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr; // sub ref
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); //add ref count

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	wsaBuf.len = len32(_recvBuffer);

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) // 팬딩 에러가 아니면 문제 있는 상황
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // sub ref 
		}
	}

}

void Session::RegisterSend(SendEvent* sendEvent)
{
	if (IsConnected() == false)
		return;


	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	if (SOCKET_ERROR == ::WSASend(_socket, &wsaBuf, 1, OUT & numOfBytes, 0, reinterpret_cast<OVERLAPPED*>(sendEvent), nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent->owner = nullptr; // sub ref count
			Xdelete(sendEvent);
		}
	}
}

void Session::ProcessConnect()
{
	_connectEvent.owner = nullptr;  // sub ref
	_connected.store(true);

	GetService()->AddSession(GetSessionRef());

	OnConnected();

	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr;
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr; // sub ref count

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	OnRecv(_recvBuffer, numOfBytes);

	RegisterRecv();
}

void Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes)
{
	sendEvent->owner = nullptr; // sub ref count;
	Xdelete(sendEvent);

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	OnSend(numOfBytes);
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// console 출력은 thread-safe 하지만 컨텍스트 스위칭 때문에
		// 로그도 일감 형태로 바꿔서 로그용 스레드로 관리할 수도 있다.
		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}
