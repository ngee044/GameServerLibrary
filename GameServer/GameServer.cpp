#include "pch.h"
#include "CorePch.h"

#include <Windows.h>
#include <iostream>

#include "ThreadManager.h"
#include "SocketUtils.h"
#include "Listener.h"

int main()
{
	Listener listener;
	listener.start_accept(NetAddress(L"127.0.0.1", 7777));

	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->launch([=]()
			{
				while (true)
				{
					g_iocp_core.iocp_dispatch();
				}
			});
	}

	GThreadManager->join();

}




