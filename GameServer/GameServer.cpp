#include "pch.h"
#include "CorePch.h"

#include <Windows.h>
#include <iostream>

#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"


int main()
{
	auto service = std::make_shared<ServerService>(NetAddress(L"127.0.0.1", 7777), std::make_shared<IocpCore>(),
		std::make_shared<Session>(), // TODO Session Manager
		100);

	ASSERT_CRASH(service->start());

	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->launch([=]()
			{
				while (true)
				{
					service->get_iocp_core()->iocp_dispatch();
				}
			});
	}

	GThreadManager->join();

}




