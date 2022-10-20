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

const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recv_buffer[BUFSIZE] = {};
	int32 recv_bytes = 0;
	int32 send_bytes = 0;
	WSAOVERLAPPED overlapped = {};
};

int main()
{
#if 0
	//���� �ʱ�ȭ (ws2_32 ���̺귯�� �ʱ�ȭ)
	//���� ���� wsa_data ä����
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
	server_addr.sin_addr.s_addr = ::htonl(INADDR_ANY); // �˾Ƽ� ����~
	server_addr.sin_port = ::htons(7777); //host to network short // 80 : HTTP


	if (::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		error_message();
		return 0;
	}

	if (::listen(listen_socket, 10) == SOCKET_ERROR) // ��⿭ ����
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
#elif 0
	WSAData wsa_data;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		return 0;
	}

	SOCKET server_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (server_socket == INVALID_SOCKET)
	{
		std::cout << __func__ << __LINE__ << std::endl;
		return 0;
	}

	SOCKADDR_IN server_addr;
	::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	server_addr.sin_port = ::htons(7777);

	if (::bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		std::cout << __func__ << __LINE__ << std::endl;
		return 0;
	}

	while (true)
	{
		SOCKADDR_IN client_addr;
		::memset(&client_addr, 0, sizeof(client_addr));
		int32 addr_len = sizeof(client_addr);

		char recv_buffer[1000];

		int32 recv_len = ::recvfrom(server_socket, recv_buffer, sizeof(recv_buffer), 0, (SOCKADDR*)&client_addr, &addr_len);
		if (recv_len <= 0)
		{
			return 0;
		}

		std::cout << "Recv Data! data = " << recv_buffer << std::endl;
		std::cout << "Recv Data! len = " << recv_len << std::endl;

		int32 error_code = ::sendto(server_socket, recv_buffer, recv_len, 0, (SOCKADDR*)&client_addr, sizeof(client_addr));

		if (error_code == SOCKET_ERROR)
		{
			std::cout << __func__ << __LINE__ << std::endl;
			return 0;
		}
		std::cout << "Send Data! Len = " << recv_len << std::endl;
	}

	::WSACleanup();
	return 0;
#elif 0
	WSAData wsa_data;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		return 0;
	}

	//���ŷ ����
	//accecpt -> ������ Ŭ�� ���� ��
	//connect -> ���� �������� ��
	//send, sendto -> ��û�� �����͸� �۽� ���ۿ� �������� ��
	//recv, recvfrom -> ���� ���ۿ� ������ �����Ͱ� �ְ�, �̸� �������� ���ۿ� �������� ��

	//����ŷ(Non-Blocking)

	SOCKET listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		return 0;
	}

	u_long on = 1;
	if (::ioctlsocket(listen_socket, FIONBIO, &on) == INVALID_SOCKET)
	{
		return 0;
	}

	SOCKADDR_IN server_addr;
	::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	server_addr.sin_port = ::htons(7777);

	if (::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		return 0;
	}

	if (::listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		return 0;
	}

	std::cout << "Accept" << endl;

	SOCKADDR_IN client_addr;
	int32 addr_len = sizeof(client_addr);

	while (true)
	{
		SOCKET client_socket = ::accept(listen_socket, (SOCKADDR*)&client_addr, &addr_len);
		if (client_socket == INVALID_SOCKET)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			//Error
			break;
		}

		std::cout << "client connected!" << std::endl;

		//Recv
		while (true)
		{
			char recv_buffer[1000];
			int32 recv_len = ::recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
			if (recv_len == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}
				//Error
				break;
			}
			else if (recv_len == 0)
			{
				//disconnect (�������)
				break;
			}

			std::cout << "Recv Data Len = " << recv_len << std::endl;

			//send
			while (true)
			{
				if (::send(client_socket, recv_buffer, recv_len, 0) == SOCKET_ERROR)
				{
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
					{
						continue;
					}
					//Error
					break;
				}

				std::cout << "Send Data ! Len = " << recv_len << std::endl;
				break;
			}
		}
	}

	::WSACleanup();

#else
	WSAData wsa_data;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		return 0;
	}

	SOCKET listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		return 0;
	}

	u_long on = 1;
	if (::ioctlsocket(listen_socket, FIONBIO, &on) == INVALID_SOCKET)
	{
		return 0;
	}

	SOCKADDR_IN server_addr;
	::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	server_addr.sin_port = ::htons(7777);

	if (::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		return 0;
	}

	if (::listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		return 0;
	}

	std::cout << "Accept" << endl;

	//Overlapped IO (�񵿱� + ����ŷ)
	//-Overlapped �Լ��� �Ǵ� (WSARecv, WSASend)
	//-Overlapped �Լ��� �����ߴ��� Ȯ�� ��
	//->���������� ��� �� ó��
	//->���������� ������ Ȯ��

	// 1) �񵿱� ����� ����
	// 2) WSABUF �迭�� ���� �ּ� + ����
	// 3) ������/ ���� ����Ʈ ��
	// 4) �� �ɼ��ε� 0
	// 5) WSAOVERLAPPED ����ü �ּҰ�
	// 6) ������� �Ϸ�Ǹ� OS�� ȣ���� �ݹ� �Լ�

	//WSASend
	//WSARecv

	//Overlapped �� (�̺�Ʈ ���)
	//-�񵿱� ����� �����ϴ� ���� ���� + ���� �ޱ� ���� �̺�Ʈ ��ü ����
	//-�񵿱� ����� �Լ� ȣ�� (1���� ���� �̺�Ʈ ��ü�� ���� �Ѱ���)
	//-�񵿱� �۾��� �ٷ� �Ϸ���� ������, WSA_IO_PENDING ���� �ڵ�
	// �ü���� �̺�Ʈ ��ü�� signaled ���·� ���� �Ϸ� ���� �˷���
	// WSAWaitForMultipleEvents �Լ� ȣ���ؼ� �̺�Ʈ ��ü�� signal �Ǻ�
	// WSAGetOverlappedResult ȣ���ؼ� �񵿱� ����� ��� Ȯ�� �� ������ ó��

	//1) �񵿱� ����
	//2) �Ѱ��� overlapped ����ü
	//3) ���۵� ����Ʈ ��
	//4) �񵿱� ����� �۾��� ���������� �������?

	//false
	//5) �񵿱� ����� �۾� ���� �ΰ� ����. ���� ��� �� ��
	//WSAGetOverlappedResult


	while (true)
	{
		SOCKADDR_IN client_addr;
		int32 addr_len = sizeof(client_addr);

		SOCKET client_socket;

		while (true)
		{
			client_socket = ::accept(listen_socket, (SOCKADDR*)&client_addr, &addr_len);
			if (client_socket != INVALID_SOCKET)
			{
				break;
			}

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			//���� �ִ� ��Ȳ
			return 0;
		}

		Session session = Session{ client_socket };
		WSAEVENT wsa_event = ::WSACreateEvent();
		session.overlapped.hEvent = wsa_event;

		std::cout << "Client Connected !" << std::endl;

		while (true)
		{
			WSABUF wsa_buf;
			wsa_buf.buf = session.recv_buffer;
			wsa_buf.len = BUFSIZE;

			DWORD recv_len = 0;
			DWORD flags = 0;

			if (::WSARecv(client_socket, &wsa_buf, 1, &recv_len, &flags, &session.overlapped, nullptr) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					//Pending
					::WSAWaitForMultipleEvents(1, &wsa_event, TRUE, WSA_INFINITE, FALSE);
					::WSAGetOverlappedResult(session.socket, &session.overlapped, &recv_len, FALSE, &flags);
				}
				else
				{
					break;
				}
			}

			std::cout << "Data Recv Len = " << recv_len << std::endl;
		}
	}
	

	//���� ����
	::WSACleanup();
#endif
}




