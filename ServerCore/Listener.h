#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
class ServerService;
class Service;

class Listener : public IocpObject
{
public:
	Listener() = default;
	virtual ~Listener();

	bool start_accept(std::shared_ptr<ServerService> service);
	void close_socket();

	virtual HANDLE get_handle() override;
	virtual void iocp_dispatch(class IocpEvent* iocp_event, int32 num_of_bytes = 0) override;

protected:
	SOCKET socket_ = INVALID_SOCKET;
	std::vector<AcceptEvent*> v_accept_events_;
	std::shared_ptr<ServerService> service_;

private:
	void register_accept(AcceptEvent* accept_event);
	void process_accept(AcceptEvent* accept_event);


};

