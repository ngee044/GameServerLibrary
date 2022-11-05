#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX SocketUtils::connect_ex = nullptr;
LPFN_DISCONNECTEX SocketUtils::disconnect_ex = nullptr;
LPFN_ACCEPTEX SocketUtils::accept_ex = nullptr;

void SocketUtils::init()
{
	std::cout << "socket utils " << __func__ << std::endl;

	WSADATA wsa_data;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsa_data) == 0);

	//런타임에 주소 얻어오는 API
	SOCKET dummy_socket = create_socket();
	ASSERT_CRASH(bind_windows_function(dummy_socket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&connect_ex)));
	ASSERT_CRASH(bind_windows_function(dummy_socket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&connect_ex)));
	ASSERT_CRASH(bind_windows_function(dummy_socket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&connect_ex)));
	close(dummy_socket);
}

void SocketUtils::clear()
{
	::WSACleanup();
}

SOCKET SocketUtils::create_socket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtils::bind_windows_function(SOCKET socket, GUID guid, LPVOID* fn)
{
	//connnect, disconnect, accept 을 런타임에 불러오는 함수
	DWORD bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), 
		fn, sizeof(*fn), OUT & bytes, NULL, NULL);
}

bool SocketUtils::set_linger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_linger = linger;
	option.l_onoff = onoff;

	return set_sock_opt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtils::set_reuse_address(SOCKET socket, bool flag)
{
	return set_sock_opt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::set_recive_buffer_size(SOCKET socket, int32 size)
{
	return set_sock_opt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::set_send_buffer_size(SOCKET socket, int32 size)
{
	return set_sock_opt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::set_tcp_no_delay(SOCKET socket, bool flag)
{
	return set_sock_opt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

//ListenSocket의 특성을 client socket에 그대로 적용
bool SocketUtils::set_update_accept_socket(SOCKET socket, SOCKET listen_socket)
{
	return set_sock_opt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listen_socket);
}

bool SocketUtils::bind(SOCKET socket, NetAddress net_addr)
{
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&net_addr.get_sock_addr()), sizeof(SOCKADDR_IN));
}

bool SocketUtils::bind_any_address(SOCKET socket, uint16 port)
{
	SOCKADDR_IN my_address;
	my_address.sin_family = AF_INET;
	my_address.sin_addr.s_addr = ::htonl(INADDR_ANY);
	my_address.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&my_address), sizeof(my_address));
}

bool SocketUtils::listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

void SocketUtils::close(SOCKET& socket)
{
	if(socket != INVALID_SOCKET)
		::closesocket(socket);
	
	socket = INVALID_SOCKET;
}
