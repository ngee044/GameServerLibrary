#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "IocpEvent.h"

class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();

	void set_net_address(NetAddress address) { net_address = address; }
	NetAddress get_net_address() { return net_address; }
	SOCKET get_socket() { return socket_; }

	virtual HANDLE get_handle() override;
	virtual void iocp_dispatch(class IocpEvent* iocp_event, int32 num_of_bytes = 0) override;

	char recv_buffer_[1000];

protected:


private:
	SOCKET socket_ = INVALID_SOCKET;
	NetAddress net_address = {};
	std::atomic<bool> connected_ = false;
};

