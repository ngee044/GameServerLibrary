#pragma once


class IocpObject;
class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	Recv,
	Send
};

/*
virtual 소멸자 사용을 금지한다. (OVERLAPPED 클래스의 다른 내용이 채워 지는 문제)
*/
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void init();

	EventType event_type_;
	std::shared_ptr<IocpObject> owner;

};

/*
*/
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}

private:

};

/*
*/
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

	std::shared_ptr<Session> session_ = nullptr;
};

/*
*/
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}

};

/*
*/
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}

};

