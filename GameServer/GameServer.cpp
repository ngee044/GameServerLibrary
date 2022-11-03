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
	
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT,
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	int32 type = 0;
};

void worker_thread_main(HANDLE iocp_handle)
{
	while (true)
	{
		DWORD byte_transfer = 0;
		Session* session = nullptr;
		OverlappedEx* overlapped_ex = nullptr;
		
		//this thread wait
		BOOL ret = ::GetQueuedCompletionStatus(iocp_handle, &byte_transfer, (ULONG_PTR*)&session, (LPOVERLAPPED*)&overlapped_ex, INFINITE);

		if (ret == FALSE || byte_transfer == 0)
		{
			continue;
		}

		ASSERT_CRASH(overlapped_ex->type == IO_TYPE::READ);

		std::cout << "Recv Data IOCP = " << byte_transfer << std::endl;

		WSABUF wsa_buf;
		wsa_buf.buf = session->recv_buffer;
		wsa_buf.len = BUFSIZE;

		DWORD recv_len = 0;
		DWORD flags = 0;
		::WSARecv(session->socket, &wsa_buf, 1, &recv_len, &flags, &overlapped_ex->overlapped, NULL);
	}
}

int main()
{
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

	//u_long on = 1;
	//if (::ioctlsocket(listen_socket, FIONBIO, &on) == INVALID_SOCKET)
	//{
	//	return 0;
	//}

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

	//IOCP
	//CP 생성
	HANDLE iocp_handle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	std::vector<Session*> session_manager;

	//TODO: worker threads
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->launch([=]() { worker_thread_main(iocp_handle); });
	}


	while (true)
	{
		SOCKADDR_IN client_addr;
		int32 addr_len = sizeof(client_addr);

		SOCKET client_socket;
		client_socket = ::accept(listen_socket, (SOCKADDR*)&client_addr, &addr_len);
		if (client_socket == INVALID_SOCKET)
		{
			return 0;
		}

		Session* session = new Session();
		session->socket = client_socket;
		session_manager.push_back(session);

		std::cout << "Client Connected !" << std::endl;

		//소켓을 CP에 등록
		::CreateIoCompletionPort((HANDLE)client_socket, iocp_handle, (ULONG_PTR)session, 0);
		
		WSABUF wsa_buf;
		wsa_buf.buf = session->recv_buffer;
		wsa_buf.len = BUFSIZE;

		OverlappedEx* overlapped_ex = new OverlappedEx();
		overlapped_ex->type = IO_TYPE::READ;

		DWORD recv_len = 0;
		DWORD flags = 0;
		::WSARecv(client_socket, &wsa_buf, 1, &recv_len, &flags, &overlapped_ex->overlapped, NULL);

		//auto s = session_manager.back();
		//session_manager.pop_back();
		//delete s;
	}

	GThreadManager->join();

	//윈속 종료
	::WSACleanup();
}




