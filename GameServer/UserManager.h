#pragma once
#include <mutex>


class User
{
	//TODO
};


class UserManager
{
public:
	static UserManager* Instance()
	{
		static UserManager instance;
		return &instance;
	}

	User* GetUser(int32 id)
	{
		std::lock_guard<std::mutex> lck(mutex_);
		return nullptr;
	}

	void ProcessSave();


private:
	std::mutex mutex_;
};

