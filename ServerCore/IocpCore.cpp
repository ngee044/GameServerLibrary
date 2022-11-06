#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore::IocpCore()
{
	iocp_handle_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(iocp_handle_ != INVALID_HANDLE_VALUE);

}

IocpCore::~IocpCore()
{
	::CloseHandle(iocp_handle_);
}

bool IocpCore::iocp_register(class IocpObject* iocp_object)
{
	return ::CreateIoCompletionPort(iocp_object->get_handle(), iocp_handle_, reinterpret_cast<ULONG_PTR>(iocp_handle_), 0);
}

bool IocpCore::iocp_dispatch(uint32 time_out)
{
	DWORD num_of_bytes = 0;
	IocpObject* iocp_object = nullptr;
	IocpEvent* iocp_event = nullptr;

	if (::GetQueuedCompletionStatus(iocp_handle_, OUT & num_of_bytes, OUT reinterpret_cast<PULONG_PTR>(&iocp_object), OUT reinterpret_cast<LPOVERLAPPED*>(&iocp_event), time_out))
	{
		iocp_object->iocp_dispatch(iocp_event, num_of_bytes);
	}
	else
	{
		int32 err_code = ::WSAGetLastError();
		switch (err_code)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			//TODO
			iocp_object->iocp_dispatch(iocp_event, num_of_bytes);
			break;
		}
	}

	return true;
}
