#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"

Session::Session()
{
	socket_ = SocketUtils::create_socket();
}

Session::~Session()
{
	SocketUtils::close(socket_);
}

HANDLE Session::get_handle()
{
	return reinterpret_cast<HANDLE>(socket_);
}

void Session::iocp_dispatch(IocpEvent* iocp_event, int32 num_of_bytes)
{
	//TODO
}
