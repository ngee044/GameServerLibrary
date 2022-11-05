#pragma once
#include "NetAddress.h"

class SocketUtils
{
public:
	static LPFN_CONNECTEX connect_ex;
	static LPFN_DISCONNECTEX disconnect_ex;
	static LPFN_ACCEPTEX accept_ex;

	static void init();
	static void clear();

	static SOCKET create_socket();
	static bool bind_windows_function(SOCKET socket, GUID guid, LPVOID* fn);

	static bool set_linger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool set_reuse_address(SOCKET socket, bool flag);
	static bool set_recive_buffer_size(SOCKET socket, int32 size);
	static bool set_send_buffer_size(SOCKET socket, int32 size);
	static bool set_tcp_no_delay(SOCKET socket, bool flag);
	static bool set_update_accept_socket(SOCKET socket, SOCKET listen_socket);

	static bool bind(SOCKET socket, NetAddress net_addr);
	static bool bind_any_address(SOCKET socket, uint16 port);
	static bool listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void  close(SOCKET& socket);
};

template<typename T>
static inline bool set_sock_opt(SOCKET socket, int32 level, int32 opt_name, T opt_val)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, opt_name, reinterpret_cast<char*>(&opt_val), sizeof(T));
}
