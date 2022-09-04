#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, FnSessionFactory fnFactory, int32 maxSessionCount)
	:_type(type), _netAddress(address), _iocpCore(core), _fnSessionFactory(fnFactory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{

}

void Service::CloseService()
{
}

SessionRef Service::CreateSession()
{
	SessionRef session = _fnSessionFactory();

	if (_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(SessionRef session)
{
	WRITE_LOCK;
	_sessionCount++;
	_sessions.insert(session);
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}


// ==========================================
//		ClientService
// ==========================================
ClientService::ClientService(NetAddress targetAddr, IocpCoreRef core, FnSessionFactory fnSessionFactory, int32 maxSessionCount)
	:Service(ServiceType::Client, targetAddr, core, fnSessionFactory, maxSessionCount)
{
}

bool ClientService::Start()
{
	return true;
}


// ==========================================
//		ServerService
// ==========================================
ServerService::ServerService(NetAddress address, IocpCoreRef core, FnSessionFactory fnSessionFactory, int32 maxSessionCount)
	:Service(ServiceType::Server, address, core, fnSessionFactory, maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if(_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	Service::CloseService();
}