#pragma once

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv, // ���� ����Ʈ recv�� ���õ� ��� ����� ���ȴ�.
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
