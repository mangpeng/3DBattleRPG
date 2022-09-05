#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"


#pragma region Session

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
		return;

	{
		//// ���� : recv�� �ٸ��� send�� ���� ���(��Ƽ������)�� ȣ��ȴ�.
		//// 1. ���� ������ ��� ����?
		//// 2. sendEvent ���� ? ����? ������? WSASend ��ø?

		//// TEMP
		//SendEvent* sendEvent = Xnew<SendEvent>();
		//sendEvent->owner = shared_from_this(); // add ref
		//sendEvent->buffer.resize(len);
		//::memcpy(sendEvent->buffer.data(), buffer, len);


		//WRITE_LOCK; 
		//// WSASend�Լ��� thread-unsafe �ϴ�.
		//// WSASend�� thread-unsafe �ϱ� ������ �����ʿ�.
		//// 1.
		//// WSASend������ ���� ������ �ǳ�(pending �ǰų� �ϸ鼭)
		//// ��Ŀ �����忡 ���� GetQueuedCompletionStatus �Ǵ� ������ ������ ������ ������.
		//// 2.
		//// pending�� ��ٴ°� �̹� Ŀ�� sendbuffer�� �� á�ٴ� ���� �ǹ��ϴµ�
		//// �̋� ����ؼ� send�ϴ� ���� ���� ���ΰ�?
		//// 3.
		//// scatter->gather�� �̿��Ͽ� wsabuf�� ��Ƽ� ������ ���� ȿ�����̴�.
		//RegisterSend(sendEvent);
	}


	//{
	//	// ���������� send�� ���ÿ� ȣ���Ͽ� ������ WSASend�� ȣ��ǰ� ���� �ʰ�
	//	// register -> send -> process -> queue ���� �����Ͱ� ���� �ִٸ� �ٽ� -> register �� ����
	//	// ��Ƽ �����忡�� �ϳ��� queue�� �ϰ��� ������, send �帧�� �ϳ��� ����

	//	WRITE_LOCK;

	//	_sendQueue.push(sendBuffer);
	//	if (_sendRegistered.exchange(true) == false)
	//		RegisterSend();
	//}

	{
		// lock�� ������ ������!
		bool registerSend = false;

		{
			WRITE_LOCK;

			_sendQueue.push(sendBuffer);

			if (_sendRegistered.exchange(true) == false)
				registerSend = true;
		}

		if(registerSend)
			RegisterSend();
	}
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
		ProcessSend(numOfBytes);
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

	if (SocketUtils::BindAnyAddress(_socket, /*������*/ 0) == false)
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

	// TF_REUSE_SOCKET: ���� AcceptEx, ConnectEx�� �ش� ������ ������ �� �ִ�.
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
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) // �ҵ� ������ �ƴϸ� ���� �ִ� ��Ȳ
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // sub ref 
		}
	}

}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this(); // add ref

	// ���� �����͸� sendEvent�� ���
	{
		WRITE_LOCK;
		
		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO �����Ͱ� �ʹ� ���� ��� ���� ó��
			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// scatter-gather
	Xvector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = (ULONG)sendBuffer->WriteSize();
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, reinterpret_cast<OVERLAPPED*>(&_sendEvent), nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr; // sub ref count
			_sendEvent.sendBuffers.clear(); // sub ref count
			_sendRegistered.store(false);
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

	OnDisconnected(); // ������ �ڵ忡�� �������̵�
	GetService()->ReleaseSession(GetSessionRef());

}

// recv�� register->Check CP -> Process -> register
// �� ���� �����ϹǷ� ��Ƽ������ ȯ�� ������� �ʾƵ� �ȴ�.
void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr; // sub ref count

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	_recvBuffer.Clean();

	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr; // sub ref
	_sendEvent.sendBuffers.clear(); // sub ref 

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	OnSend(numOfBytes);

	WRITE_LOCK;
	if(_sendQueue.empty())
		_sendRegistered.store(false);
	else // send�� ó���Ǵ� ���� �ٽ� �ϰ��� ���� ���
		RegisterSend();
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
		// console ����� thread-safe ������ ���ؽ�Ʈ ����Ī ������
		// �α׵� �ϰ� ���·� �ٲ㼭 �α׿� ������� ������ ���� �ִ�.
		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}

#pragma endregion

#pragma region PacketSession

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSzie = len - processLen;

		// �ּ����� ����� �Ľ� ��������
		if (dataSzie < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(buffer + processLen));

		// ����� ��ϵ� ��Ŷ �����ŭ �����Ͱ� �Դ���
		if (dataSzie < header.size)
			break;

		// ��Ŷ ���� ����
		OnRecvPacket(buffer, header.size);

		processLen += header.size;
	}

	return processLen;
}

#pragma endregion 