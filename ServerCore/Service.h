#pragma once
#include "NetAddress.h"
#include "IocpCore.h"

#include <functional>

class Session;
class Listener;


enum class ServiceType : uint8
{
	Server,
	Client
};


using SessionFactory = std::function<std::shared_ptr<Session>(void)>;

class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(ServiceType service_type, NetAddress net_address, std::shared_ptr<IocpCore> iocp_core, SessionFactory session_factory, int32 max_session_count = 1);
	virtual ~Service();

	virtual bool start() abstract;
	std::shared_ptr<Session> create_session();
	virtual void close_service();

	void add_session(std::shared_ptr<Session> session);
	void release_session(std::shared_ptr<Session> session);
	
	void set_session_factory(SessionFactory func) { session_factory_ = func; }
	
	bool can_start() { return session_factory_ != nullptr; }
	int get_current_session_count() { return session_count_; }
	int get_max_session_count() { return max_session_count_; }
	ServiceType get_service_type() { return service_type_; }
	NetAddress get_net_address() { return net_address_; }
	std::shared_ptr<IocpCore>& get_iocp_core() { return iocp_core_; }

protected:
	USE_LOCK;
	ServiceType service_type_;
	NetAddress net_address_ = {};
	std::shared_ptr<IocpCore> iocp_core_;

	std::set<std::shared_ptr<Session>> sessions_;
	int32 session_count_ = 0;
	int32 max_session_count_ = 0;
	SessionFactory session_factory_;

};


/*
*/
class ClientService : public Service
{
public:
	ClientService(NetAddress target_net_address, std::shared_ptr<IocpCore> iocp_core, SessionFactory session_factory, int32 max_session_count);
	virtual ~ClientService() {}

	virtual bool start() override;
};


/*
*/
class ServerService : public Service
{
public:
	ServerService(NetAddress net_address, std::shared_ptr<IocpCore> iocp_core, SessionFactory session_factory, int32 max_session_count);
	virtual ~ServerService() {}

	virtual bool start() override;
	virtual void close_service() override;


private:
	std::shared_ptr<Listener> listener_ = nullptr;
};