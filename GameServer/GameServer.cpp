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

	//Overlapped IO (비동기 + 논블로킹)
	//-Overlapped 함수를 건다 (WSARecv, WSASend)
	//-Overlapped 함수가 성공했는지 확인 후
	//->성공했으면 결과 얻어서 처리
	//->실패했으면 사유를 확인

	// 1) 비동기 입출력 소켓
	// 2) WSABUF 배열의 시작 주소 + 개수
	// 3) 보내고/ 받은 바이트 수
	// 4) 상세 옵션인데 0
	// 5) WSAOVERLAPPED 구조체 주소값
	// 6) 입출력이 완료되면 OS가 호출할 콜백 함수

	//WSASend
	//WSARecv

	//Overlapped 모델 (이벤트 기반)
	//-비동기 입출력 지원하는 소켓 생성 + 통지 받기 위한 이벤트 객체 생성
	//-비동기 입출력 함수 호출 (1에서 만든 이벤트 객체를 같이 넘겨줌)
	//-비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드
	// 운영체제는 이벤트 객체를 signaled 상태로 만들어서 완료 상태 알려줌
	// WSAWaitForMultipleEvents 함수 호출해서 이벤트 객체의 signal 판별
	// WSAGetOverlappedResult 호출해서 비동기 입출력 결과 확인 및 데이터 처리

	//1) 비동기 소켓
	//2) 넘겨준 overlapped 구조체
	//3) 전송된 바이트 수
	//4) 비동기 입출력 작업이 끝날때까지 대기할지?

	//false
	//5) 비동기 입출력 작업 관련 부가 정보. 거의 사용 안 함
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

			//문제 있는 상황
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
	

	//윈속 종료
	::WSACleanup();
#endif
}




