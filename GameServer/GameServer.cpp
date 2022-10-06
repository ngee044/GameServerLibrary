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

int main()
{
#if 0
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

	//블로킹 소켓
	//accecpt -> 접속한 클라가 있을 때
	//connect -> 접속 성공했을 때
	//send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	//recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고, 이를 유저레벨 버퍼에 복사했을 때

	//논블로킹(Non-Blocking)

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
				//disconnect (연결끊김)
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

	//Select 모델 = (select 함수가 핵심이 되는)
	//소켓 함수 호출이 성공할 시점을 미리 알 수 있다.
	//문제 상황
	//수신버퍼에 데이터가 없는데 read한다거나
	//송신버퍼가 꽉 찼는데 write 한다거나!
	//블로킹 소켓: 조건이 만족되지 않아서 블로킹되는 상황 예방
	//논블로킹 소켓: 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방

	//socket set
	//읽기 쓰기 예외 관찰 대상 등록
	//OutBand는 send() 마지막 인자 MSG_OOB로 보내는 특별한 데이터
	//받는 쪽에서도 recv OOB 세팅을 해야 읽을 수 있음
	//2)select(readSet, writeSet, exceptSet): -> 관찰 시작
	//3)적어도 하나의 소켓이 준비되면 리턴 -> 낙오자는 알아서 제거됨
	//4)남은 소켓 체크해서 진행

	//fd_set read: 
	//FD_ZERO(set): 비운다
	//FD_SET : 소켓 세팅
	//FD_CLR: 소켓을 제거
	//FD_ISSET: 소켓 

	std::vector<Session> sessions;
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
		//소켓 셋 초기화
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		//ListenSocket 등록
		FD_SET(listen_socket, &reads);

		for (auto& s : sessions)
		{
			if (s.recv_bytes <= s.send_bytes)
			{
				FD_SET(s.socket, &reads);
			}
			else
			{
				FD_SET(s.socket, &writes);
			}
		}

		//[옵션] 마지막 timeout 인자 설정 가능
		//nullptr 인 경우 무한 대기
		auto ret = ::select(0, &reads, &writes, nullptr, nullptr);
		if (ret == SOCKET_ERROR)
		{
			break;
		}

		//Listener 소켓 check
		if (FD_ISSET(listen_socket, &reads))
		{
			SOCKADDR_IN client_addr;
			int32 addr_len = sizeof(client_addr);
			SOCKET client_socket = ::accept(listen_socket, (SOCKADDR*)&client_addr, &addr_len);
			if (client_socket != INVALID_SOCKET)
			{
				std::cout << "client connected" << std::endl;
				sessions.push_back(Session{ client_socket });
			}
		}

		//나머지 소켓 체크
		for (auto& s : sessions)
		{
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recv_len = ::recv(s.socket, s.recv_buffer, BUFSIZE, 0);
				if (recv_len <= 0)
				{
					//TODO : sessions delete
					continue;
				}

				s.recv_bytes = recv_len;
			}

			//write
			if (FD_ISSET(s.socket, &writes))
			{
				//블로킹 모드 -> 모든 데이터 다 보냄
				//논블로킹 모드 -> 일부만 보낼 수 가 있음 (상대방 수신 버퍼 상황에 따라)
				int32 send_len = ::send(s.socket, &s.recv_buffer[s.send_bytes], s.recv_bytes - s.send_bytes, 0);
				if (send_len == SOCKET_ERROR)
				{
					continue;
				}

				s.send_bytes += send_len;
				if (s.recv_bytes == s.send_bytes)
				{
					s.recv_bytes = 0;
					s.send_bytes = 0;
				}
			}
		}


	}

	//윈속 종료
	::WSACleanup();
#endif
}




