#pragma once


class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	Recv,
	Send
};

/*
virtual �Ҹ��� ����� �����Ѵ�. (OVERLAPPED Ŭ������ �ٸ� ������ ä�� ���� ����)
*/
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void init();
	EventType get_type() { return type_; }

protected:
	EventType type_;

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

	void set_session(Session* session) { session_ = session; }
	Session* get_session() { return session_; }

private:
	Session* session_ = nullptr;
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

