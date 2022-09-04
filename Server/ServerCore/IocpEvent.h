#pragma once

enum class EventType : uint8
{
	Connect,
	Disconnect,
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

public:
	EventType eventType;
	IocpObjectRef owner;
};


class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};



class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

public:
	SessionRef	session = nullptr;

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
	vector<BYTE> buffer;
};
