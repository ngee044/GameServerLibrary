#pragma once


class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sock_addr);
	NetAddress(std::wstring ip, uint16 port);

	SOCKADDR_IN& get_sock_addr() { return sock_addr_; }
	std::wstring get_ip_address();
	uint16 get_port() { return ::ntohs(sock_addr_.sin_port); }

	static IN_ADDR ip_address(const WCHAR* ip);

private:
	SOCKADDR_IN sock_addr_ = {};
};

