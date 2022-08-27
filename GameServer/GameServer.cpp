#include "pch.h"

#include <Windows.h>
#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#include "CoreMacro.h"
#include "ThreadManager.h"
#include "Memory.h"

#pragma comment(lib, "ws2_32.lib")

void error_message()
{
	int32 err_code = ::WSAGetLastError();
	std::cout << "server" << std::endl;
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

	SOCKET listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		error_message();
		return 0;
	}

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 알아서 해줘~
	server_addr.sin_port = ::htons(7777); //host to network short // 80 : HTTP


	if (::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		error_message();
		return 0;
	}

	if (::listen(listen_socket, 10) == SOCKET_ERROR) // 대기열 설정
	{
		error_message();
		return 0;
	}

	while (true)
	{ 
		SOCKADDR_IN client_addr; //IPv4
		memset(&client_addr, 0, sizeof(client_addr));

		int32 addr_len = sizeof(client_addr);
		SOCKET client_socket = accept(listen_socket, (SOCKADDR*)&client_addr, &addr_len);
		if (client_socket == INVALID_SOCKET)
		{
			error_message();
			return 0;
		}

		char ip_address[16];
		::inet_ntop(AF_INET, &client_addr.sin_addr, ip_address, sizeof(ip_address));
		std::cout << "Client Coinnected! iP = " << ip_address << std::endl;

		//TODO

		while (true)
		{
			char recv_buffer[1000];

			auto recv_len = ::recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
			if (recv_len <= 0)
			{
				error_message();
				return 0;
			
			}

			cout << "Recv Data! data = " << recv_buffer << endl;
			cout << "Recv Data! len = " << recv_len << endl;

			auto result = ::send(client_socket, recv_buffer, recv_len, 0);
			if (result == SOCKET_ERROR)
			{
				error_message();
				return 0;
			}

		}

	}


	::WSACleanup();
	return 0;
}

