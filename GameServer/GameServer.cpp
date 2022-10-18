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

	// WSAEventSelect = (WSAEventSelect함수가 핵심이 되는)
	// 소켓과 관련된 네트워크 이벤트를 [이벤트 객체]를 통해 감지

	// 이벤트 객체 관련 함수들
	// 생성 : WSACreateEvent(수동 리셋 Manual-Reset + Non-Signaled 상태 시작)
	// 삭제 : WSACloseEvent
	// 신호 상태 감지: WSAWaitForMultipleEvents
	// 구체적인 네트워크 이벤트 알아내기 : WSAEnumNetworkEvents

	// 소켓 <-> 이벤트 객체 연동
	// WSAEventSelect(socket, event, networkEvents)
	// 관심있는 네트워크 이벤트
	// FD_ACCEPT : 접속한 클라가 있음 accept
	// FD_READ : 데이터 수신 가능 recv, recvfrom
	// FD_WRITE : 데이터 송신 가능 send, sendto
	// FD_CLOSE : 상대가 접속 종료
	// FD_CONNECT : 통신을 위한 연결 절차 완료

	// 주의 사항
	// WSAEventSelect 함수를 호출하면, 해당 소켓은 자동으로 넌블로킹 모드 전환
	// accept() 함수가 리턴하는 소켓은 listenSocket과 동일한 속성을 갖는다.
	// - 따라서 clientSocket은 FD_READ, FD_WRITE 등을 다시 등록 필요
	// 드물게 WSAWOULDBLOCK 오류가 뜰 수 있으니 예외 처리 필요
	// 중요)
	// - 이벤트 발생 시, 적절한 소켓 함수 호출해야 함
	// - 아니면 다음 번에는 동일 네트워크 이벤트가 발생 X
	// ex) FD_READ 이벤트 떳으면 recv() 호출해야 하고, 안하면 FD_READ 두 번 다시 X


	// 1) count, event
	// 2) waitAll : 모두 기다림? 하나만 완료 되어도 OK?
	// 3) timeout : 타임 아웃
	// 4) 지금은 false
	// return : 완료된 첫번째 인덱스
	//WSAWaitForMultipleEvents

	// 1) socket
	// 2) eventObject : socket 과 연동된 이벤트 객체 핸들을 넘겨주면, 이벤트 객체를 non-signaled
	// 3) networkEvent : 네트워크 이벤트 / 오류 정보가 저장
	// WSAEnumNetworkEvents


	std::vector<WSAEVENT> wsa_events;
	std::vector<Session> sessions;
	sessions.reserve(100);

	WSAEVENT listen_event = ::WSACreateEvent();
	wsa_events.push_back(listen_event);
	sessions.push_back(Session{ listen_socket });
	if (::WSAEventSelect(listen_socket, listen_event, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		return 0;
	}

	while (true)
	{
		int32 index = ::WSAWaitForMultipleEvents(wsa_events.size(), &wsa_events[0], FALSE, WSA_INFINITE, FALSE);

		if (index == WSA_WAIT_FAILED)
		{
			std::cout << "wsa wait failed..." << std::endl;
			continue;
		}

		index -= WSA_WAIT_EVENT_0;

		//::WSAResetEvent(wsa_events[index]);

		WSANETWORKEVENTS network_events;
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsa_events[index], &network_events) == SOCKET_ERROR)
		{
			continue;
		}

		// Listener 소켓 체크
		if (network_events.lNetworkEvents & FD_ACCEPT)
		{
			if (network_events.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				continue;
			}

			SOCKADDR_IN client_addr;
			int32 addr_len = sizeof(client_addr);

			SOCKET client_socket = ::accept(listen_socket, (SOCKADDR*)&client_addr, &addr_len);
			if (client_socket != INVALID_SOCKET)
			{
				std::cout << "Client Connected" << endl;

				WSAEVENT client_event = ::WSACreateEvent();
				wsa_events.push_back(client_event);
				sessions.push_back(Session{ client_socket });
				if (::WSAEventSelect(client_socket, client_event, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
				{
					return 0;
				}
			}
		}

		//Client Session 소켓 체크
		if (network_events.lNetworkEvents & FD_READ || network_events.lNetworkEvents & FD_WRITE)
		{

			//Error-Check
			if ((network_events.lNetworkEvents & FD_READ) && (network_events.iErrorCode[FD_READ_BIT] != 0))
			{
				continue;
			}

			//Error-Check
			if ((network_events.lNetworkEvents & FD_WRITE) && (network_events.iErrorCode[FD_WRITE_BIT] != 0))
			{
				continue;
			}

			Session& s = sessions[index];

			//Read
			if (s.recv_bytes == 0)
			{
				int32 recv_len = ::recv(s.socket, s.recv_buffer, BUFSIZE, 0);
				if (recv_len == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					//TODO : Remove Session;
					continue;
				}

				s.recv_bytes = recv_len;
				std::cout << "recv data =" << recv_len << std::endl;
			}

			//write
			if (s.recv_bytes > s.send_bytes)
			{
				int32 send_len = ::send(s.socket, &s.recv_buffer[s.send_bytes], s.recv_bytes - s.send_bytes, 0);
				if (send_len == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					//TODO : Remove Session;
					continue;
				}

				s.send_bytes += send_len;
				if (s.recv_bytes == s.send_bytes)
				{
					s.recv_bytes = 0;
					s.send_bytes = 0;
				}

				std::cout << "send data = " << send_len << std::endl;
			}
		}

		//FD_CLOSE 처리
		if (network_events.lNetworkEvents & FD_CLOSE)
		{
			// TODO : Remove Socket

		}
	}

	//윈속 종료
	::WSACleanup();
#endif
}




