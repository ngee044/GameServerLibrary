#include "pch.h"
#include "IocpEvent.h"


IocpEvent::IocpEvent(EventType type) : type_(type)
{
	init();
}

void IocpEvent::init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
