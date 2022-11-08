#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

Service::Service(ServiceType service_type, NetAddress net_address, std::shared_ptr<IocpCore> iocp_core, SessionFactory session_factory, int32 max_session_count)
	: service_type_(service_type), 
	net_address_(net_address), 
	iocp_core_(iocp_core), 
	session_factory_(session_factory), 
	max_session_count_(max_session_count)
{
}

Service::~Service()
{
}

std::shared_ptr<Session> Service::create_session()
{
	auto session = session_factory_();
	if (!iocp_core_->iocp_register(session))
	{
		return nullptr;
	}

	return session;
}

void Service::close_service()
{
}

void Service::add_session(std::shared_ptr<Session> session)
{
	WRITE_LOCK;
	session_count_++;
	sessions_.insert(session);
}

void Service::release_session(std::shared_ptr<Session> session)
{
	WRITE_LOCK;
	ASSERT_CRASH(sessions_.erase(session) != 0);
	session_count_--;
}

ClientService::ClientService(NetAddress target_net_address, std::shared_ptr<IocpCore> iocp_core, SessionFactory session_factory, int32 max_session_count)
	: Service(ServiceType::Client, target_net_address, iocp_core, session_factory, max_session_count)
{
}

bool ClientService::start()
{
	// TODO
	return false;
}

ServerService::ServerService(NetAddress net_address, std::shared_ptr<IocpCore> iocp_core, SessionFactory session_factory, int32 max_session_count)
	: Service(ServiceType::Server, net_address, iocp_core, session_factory, max_session_count)
{

}

bool ServerService::start()
{
	if (!can_start())
	{
		return false;
	}

	listener_ = std::make_shared<Listener>();
	if (listener_ == nullptr)
	{
		return false;
	}

	std::shared_ptr<ServerService> service = std::static_pointer_cast<ServerService>(shared_from_this());
	if (!listener_->start_accept(service))
	{
		return false;
	}
	
	return true;
}

void ServerService::close_service()
{
	//TODO
	Service::close_service();

}
