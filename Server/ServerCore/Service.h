#pragma once

#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include "functional"


enum class ServiceType : uint8 
{
	Server,
	Client
};

using FnSessionFactory = function<SessionRef(void)>;

class Service : public enable_shared_from_this<Service>
{
public:
	// 서버라면 자신의 주소, 클라라면 붙어야할 대상(서버) 주소
	Service(ServiceType type, NetAddress address, IocpCoreRef core, FnSessionFactory fnFactory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool		Start() abstract;
	bool				CanStart() { return _fnSessionFactory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(FnSessionFactory func) { _fnSessionFactory = func; }

	void				BroadCast(SendBufferRef sendBuffer); // dummy client용 임시 
	SessionRef			CreateSession();
	void				AddSession(SessionRef session);
	void				ReleaseSession(SessionRef session);

	int32				GetCurrentSessionCount() { return _sessionCount; }
	int32				GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType			GetServiceType() { return _type; }
	NetAddress			GetNetAddress() { return _netAddress; }
	IocpCoreRef&		GetIocpCore() { return _iocpCore; }

protected:
	USE_LOCK;

	ServiceType			_type;
	NetAddress			_netAddress = {};
	IocpCoreRef			_iocpCore;

	Xset<SessionRef>	_sessions;
	int32				_sessionCount = 0;
	int32				_maxSessionCount = 0;
	FnSessionFactory	_fnSessionFactory;

};


class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddr, IocpCoreRef core, FnSessionFactory fnSessionFactory, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool Start() override;
};


class ServerService : public Service
{
public:
	ServerService(NetAddress address, IocpCoreRef core, FnSessionFactory fnSessionFactory, int32 maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool Start() override;
	virtual void CloseService() override;

private:
	ListenerRef _listener = nullptr;
};