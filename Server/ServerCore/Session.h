#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	Session();
	virtual ~Session();

public:
	void					Disconnect(const WCHAR* cause);

	shared_ptr<Service>		GetService() { return _service.lock(); }
	void					SetService(shared_ptr<Service> service) { _service = service; }

public:
	void			SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress		GetAddress() { return _netAddress; }
	SOCKET			GetSocket() { return _socket; }
	bool			IsConnected() { return _connected; }
	SessionRef		GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	virtual HANDLE	GetHandle() override;
	virtual void	Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
					// 서버의 경우 바로 연결 완료되면 바로 ProcessConnect를 호출 하지만,
					// 클라이언트 서비스의 경우 직접 connect를 등록 할 수 있다.
	void			RegisterConnect();  
	void			RegisterRecv();
	void			RegisterSend();

	void			ProcessConnect();
	void			ProcessRecv(int32 numOfBytes);
	void			ProcessSend(int32 numOfBytes);

	void			HandleError(int32 errorCode);

protected:
					/* 컨텐츠 코드에서 오버로딩 */
	virtual	void	OnConnected() {}
	virtual int32	OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void	OnSend(int32 len) {}
	virtual void	OnDisconnected() {}

public:
	char _recvBuffer[1000];

private:
	weak_ptr<Service>		_service; // 서버거 종료하지 않는 이상 service는 존재
	SOCKET					_socket = INVALID_SOCKET;
	NetAddress				_netAddress = {};
	Atomic<bool>			_connected = false;

private:
	USE_LOCK;

	// 수신관련

	// 송신관련

private:
	// iocpEvent 재사용
	RecvEvent			_recvEvent;
};

