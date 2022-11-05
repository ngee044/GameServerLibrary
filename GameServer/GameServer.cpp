#include "pch.h"
#include "CorePch.h"

#include <Windows.h>
#include <iostream>

#include "SocketUtils.h"

int main()
{
	auto socket = SocketUtils::create_socket();
	SocketUtils::bind_any_address(socket, 7777);
	SocketUtils::listen(socket);

	::accept(socket, nullptr, nullptr);

	std::cout << "client connected" << std::endl;

}




