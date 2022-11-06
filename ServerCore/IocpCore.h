#pragma once


class IocpObject
{
public:
	virtual HANDLE get_handle() abstract;
	virtual void iocp_dispatch(class IocpEvent* iocp_event, int32 num_of_bytes = 0) abstract;
};


class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE get_handle() { return iocp_handle_; }

	bool iocp_register(class IocpObject* iocp_object);
	bool iocp_dispatch(uint32 time_out = INFINITE);

private:
	HANDLE iocp_handle_;
	
};

extern IocpCore g_iocp_core;