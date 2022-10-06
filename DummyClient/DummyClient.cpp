#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void error_message()
{

	int32 err_code = ::WSAGetLastError();
	std::cout << "client" << std::endl;
	std::cout << "socket error code : " << err_code << std::endl;
}

int main()
{
#if 0	//왼쪽 초기화 (ws2_32 라이브러리 초기화)
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

	if (::connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		error_message();
		return 0;
	}

	cout << "Connected to Server!" << endl;

	while (true)
	{
		//TODO.......
		char send_buffer[100] = "hello world!!";
		auto result = ::send(client_socket, send_buffer, sizeof(send_buffer), 0);
		if (result == SOCKET_ERROR)
		{
			error_message();
			return 0;
		}

		cout << "send data len = " << sizeof(send_buffer) << endl;

		char recv_buffer[1000];

		auto recv_len = ::recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
		if (recv_len <= 0)
		{
			error_message();
			return 0;
		}

		cout << "Recv Data! data = " << recv_buffer << endl;
		cout << "Recv Data! len = " << recv_len << endl;

		std::this_thread::sleep_for(1s);
	}

	closesocket(client_socket);

	::WSACleanup();

	return 0;
#elif 0
	WSAData wsa_data;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		std::cout << __func__ << __LINE__ << std::endl;
		return 0;
	}

	SOCKET client_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		std::cout << __func__ << __LINE__ << std::endl;
		return 0;
	}

	SOCKADDR_IN server_addr;
	::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	server_addr.sin_port = ::htons(7777); //host to network short // 80 : HTTP


	while (true)
	{
		//TODO.......
		char send_buffer[100] = "hello world!!";
		auto result = ::sendto(client_socket, send_buffer, sizeof(send_buffer), 0, (SOCKADDR*)&server_addr, sizeof(server_addr));
		if (result == SOCKET_ERROR)
		{
			error_message();
			return 0;
		}

		std::cout << "send data len = " << sizeof(send_buffer) << endl;

		SOCKADDR_IN recv_addr;
		::memset(&recv_addr, 0, sizeof(recv_addr));
		int32 addr_len = sizeof(recv_addr);

		char recv_buffer[1000];

		int32 recv_len = ::recvfrom(client_socket, recv_buffer, sizeof(recv_buffer), 0, (SOCKADDR*)&recv_addr, &addr_len);

		if (recv_len <= 0)
		{
			error_message();
			return 0;
		}

		std::cout << "Recv Data! data = " << recv_buffer << std::endl;
		std::cout << "Recv Data! len = " << recv_len << std::endl;

		std::this_thread::sleep_for(1s);
	}

	closesocket(client_socket);

	::WSACleanup();

	return 0;
#elif 1
	WSAData wsa_data;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		return 0;
	}

	SOCKET client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		return 0;
	}

	u_long on = 1;
	if (::ioctlsocket(client_socket, FIONBIO, &on) == INVALID_SOCKET)
	{
		return 0;
	}

	SOCKADDR_IN server_addr;
	::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	server_addr.sin_port = ::htons(7777);

	//connect
	while (true)
	{
		if (::connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
		{
			//do nonblocking...
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			
			//already connected is break
			if (::WSAGetLastError() == WSAEISCONN)
			{
				break;
			}
			//error
			break;
		}
	}

	std::cout << "Connected to server!" << std::endl;

	char send_buffer[100] = "hello world";

	//send
	while (true)
	{
		if (::send(client_socket, send_buffer, sizeof(send_buffer), 0) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			//Error
			break;
		}
		std::cout << "Send Data ! Len = " << sizeof(send_buffer) << std::endl;

		//recv
		while (true)
		{
			char recv_buffer[1000];
			int32 recv_len = ::recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
			if (recv_len == SOCKET_ERROR)
			{
				//do nonblocking
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}
				//Error
				break;
			}
			else if (recv_len == 0)
			{
				//disconnect (연결끊김)
				break;
			}

			std::cout << "Recv Data Len = " << recv_len << std::endl;

			break;
		}

		std::this_thread::sleep_for(1s);
	}

	//socket resource return
	::closesocket(client_socket);

	//winsock exit
	::WSACleanup();

#else

#endif
}