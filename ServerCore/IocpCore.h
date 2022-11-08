#pragma once


class IocpObject : public std::enable_shared_from_this<IocpObject>
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

	bool iocp_register(std::shared_ptr<IocpObject> iocp_object);
	bool iocp_dispatch(uint32 time_out = INFINITE);

private:
	HANDLE iocp_handle_;
	
};

