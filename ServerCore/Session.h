#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "IocpEvent.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	Session();
	virtual ~Session();

	void disconnect(const WCHAR* cause);
	
	std::shared_ptr<Service> get_service() { return service_.lock(); }
	void set_service(std::shared_ptr<Service> serivce) { service_ = serivce; }

	void set_net_address(NetAddress address) { net_address = address; }
	NetAddress get_net_address() { return net_address; }
	SOCKET get_socket() { return socket_; }
	bool is_connected() { return connected_; }
	std::shared_ptr<Session> get_this_session() { return static_pointer_cast<Session>(shared_from_this()); }

	virtual HANDLE get_handle() override;
	virtual void iocp_dispatch(class IocpEvent* iocp_event, int32 num_of_bytes = 0) override;

	char recv_buffer_[1000];

protected:
	/*전송 관련*/
	void register_connect();
	void register_recive();
	void register_send();

	void process_connect();
	void process_recive(int32 num_of_bytes);
	void process_send(int32 num_of_bytes);

	void handle_error(int32 error_code);

	/*contents code override (virtual)*/
	virtual void on_connected() { }
	virtual int32 on_recive(BYTE* buffer, int32 len) { return len; }
	virtual void on_send(int32 len) {}
	virtual void on_disconnected() {}

private:
	USE_LOCK;

	std::weak_ptr<Service> service_;
	SOCKET socket_ = INVALID_SOCKET;
	NetAddress net_address = {};
	std::atomic<bool> connected_ = false;

	RecvEvent recive_event_;
};

