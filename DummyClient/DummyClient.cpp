#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void error_message()
{
	int32 err_code = ::WSAGetLastError();
	std::cout << "socket error code : " << err_code << std::endl;
}

int main()
{
	//왼쪽 초기화 (ws2_32 라이브러리 초기화)
	//관련 정보 wsa_data 채워짐
	WSAData wsa_data;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		return 0;
	}

	SOCKET client_socket =  ::socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		error_message();
		return 0;
	}

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	::inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	server_addr.sin_port = ::htons(7777); //host to network short // 80 : HTTP
	::WSACleanup();

	if (::connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		error_message();
		return 0;
	}

	cout << "Connected to Server!" << endl;

	while (true)
	{
		//TODO.......
		std::this_thread::sleep_for(1s);
	}

	closesocket(client_socket);

	::WSACleanup();

	return 0;
}