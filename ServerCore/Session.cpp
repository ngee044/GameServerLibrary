#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

Session::Session()
{
	socket_ = SocketUtils::create_socket();
}

Session::~Session()
{
	SocketUtils::close(socket_);
}

void Session::disconnect(const WCHAR* cause)
{
	if (!connected_.exchange(false))
	{
		return;
	}

	std::wcout << "disconnect : " << cause << std::endl;

	on_disconnected();
	SocketUtils::close(socket_);
	get_service()->release_session(get_this_session());
}

HANDLE Session::get_handle()
{
	return reinterpret_cast<HANDLE>(socket_);
}

void Session::iocp_dispatch(IocpEvent* iocp_event, int32 num_of_bytes)
{
	//TODO

	switch (iocp_event->event_type_)
	{
	case EventType::Connect:
		process_connect();
		break;
	case EventType::Recv:
		process_recive(num_of_bytes);
		break;
	case EventType::Send:
		process_send(num_of_bytes);
		break;
	}
}

void Session::register_connect()
{

}

void Session::register_recive()
{
	if (!is_connected())
	{
		return;
	}

	recive_event_.init();
	recive_event_.owner_ = shared_from_this(); // ADD_REF

	WSABUF wsa_buf;
	wsa_buf.buf = reinterpret_cast<char*>(recv_buffer_);
	wsa_buf.len = len32(recv_buffer_);

	DWORD num_of_bytes = 0;
	DWORD flags = 0;

	if (SOCKET_ERROR == ::WSARecv(socket_, &wsa_buf, 1, &num_of_bytes, &flags, &recive_event_, nullptr))
	{
		int32 error_code = ::WSAGetLastError();
		if (error_code != WSA_IO_PENDING)
		{
			handle_error(error_code);
			recive_event_.owner_ = nullptr;
		}
	}
}

void Session::register_send()
{
}

void Session::process_connect()
{
	//session register
	get_service()->add_session(get_this_session());

	//contents code is override
	on_connected();

	//recive...
	register_recive();
}

void Session::process_recive(int32 num_of_bytes)
{
	recive_event_.owner_ = nullptr;

	if (num_of_bytes == 0)
	{
		disconnect(L"Recv 0");
		return;
	}

	// TODO
	std::cout << "recive data len = " << num_of_bytes << std::endl;

	// 수신 등록
	register_recive();
}

void Session::process_send(int32 num_of_bytes)
{
}

void Session::handle_error(int32 error_code)
{
	switch (error_code)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		disconnect(L"Handle Error");
		break;
	default:
		std::cout << "Handle Error : " << error_code << std::endl;
		break;
	}
}
