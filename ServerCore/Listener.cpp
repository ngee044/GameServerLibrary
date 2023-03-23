#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

Listener::~Listener()
{
	SocketUtils::close(socket_);

	for (auto e : v_accept_events_)
	{
		xdelete(e);
	}
}

bool Listener::start_accept(std::shared_ptr<ServerService> service)
{
	service_ = service;
	if (service_ == nullptr)
	{
		return false;
	}

	socket_ = SocketUtils::create_socket();
	if (socket_ == INVALID_SOCKET)
	{
		return false;
	}

	if (service_->get_iocp_core()->iocp_register(shared_from_this()) == false)
	{
		return false;
	}

	if (SocketUtils::set_reuse_address(socket_, true) == false)
	{
		return false;
	}

	if (SocketUtils::set_linger(socket_, 0, 0) == false)
	{
		return false;
	}

	if (SocketUtils::bind(socket_, service_->get_net_address()) == false)
	{
		return false;
	}

	if (SocketUtils::listen(socket_) == false)
	{
		return false;
	}

	const int32 accept_count = service_->get_max_session_count();
	for (int32 i = 0; i < accept_count; i++)
	{
		AcceptEvent* accept_event = xnew<AcceptEvent>();
		accept_event->owner_ = shared_from_this();
		v_accept_events_.push_back(accept_event);
		register_accept(accept_event);
	}

	return true;
}

void Listener::close_socket()
{
	SocketUtils::close(socket_);
}

HANDLE Listener::get_handle()
{
	return reinterpret_cast<HANDLE>(socket_);
}

void Listener::iocp_dispatch(IocpEvent* iocp_event, int32 num_of_bytes)
{
	ASSERT_CRASH(iocp_event->event_type_ == EventType::Accept);
	AcceptEvent* accept_event = static_cast<AcceptEvent*>(iocp_event);
	process_accept(accept_event);
}

void Listener::register_accept(AcceptEvent* accept_event)
{
	auto session = service_->create_session(); //register iocp

	accept_event->init();
	accept_event->session_ = session;

	DWORD byte_recevied = 0;
	if (false == SocketUtils::accept_ex(socket_, session->get_socket(), session->recv_buffer_, 0, 
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & byte_recevied, static_cast<LPOVERLAPPED>(accept_event)))
	{
		const int32 error = ::WSAGetLastError();
		if (error == WSA_IO_PENDING)
		{
			//문제가 없음 아직 접속 시도가 없는 것
		}
		else
		{
			//일단 다시 시도 해본다
			register_accept(accept_event);
		}
	}
}

void Listener::process_accept(AcceptEvent* accept_event)
{
	auto session = accept_event->session_;

	if (false == SocketUtils::set_update_accept_socket(session->get_socket(), socket_))
	{
		register_accept(accept_event);
		return;
	}

	SOCKADDR_IN sock_address;
	int32 size_of_sock_addr = sizeof(sock_address);
	if (SOCKET_ERROR == ::getpeername(session->get_socket(), OUT reinterpret_cast<SOCKADDR*>(&sock_address), &size_of_sock_addr))
	{
		register_accept(accept_event);
		return;
	}

	session->set_net_address(NetAddress(sock_address));
	session->process_connect();

	std::cout << "Client Connected!" << std::endl;

	register_accept(accept_event);
}
