#pragma once

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv, // 제로 바이트 recv와 관련된 고급 기법에 사용된다.
	Recv,
	Send
};

class Session;

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void		Init();
	EventType	GetType() { return _type; }


protected:
	EventType _type;
};


class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};



class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

	void		SetSession(Session* session) { _session = session; }
	Session*	GetSession() { return _session; }
private:
	Session* _session = nullptr;

};


class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};


class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}
};
