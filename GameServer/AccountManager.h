#pragma once
#include <mutex>


class Account
{
	//TODO
};


class AccountManager
{
public:
	static AccountManager* Instance()
	{
		static AccountManager instance;
		return &instance;
	}

	Account* GetAccount(int32 id)
	{
		std::lock_guard<std::mutex> guard(mutex_);
		return nullptr;
	}

	void ProcessLogin();

private:
	std::mutex mutex_;
};

