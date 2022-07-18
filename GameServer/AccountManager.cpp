#include "pch.h"
#include "AccountManager.h"

void AccountManager::ProcessLogin()
{
	std::lock_guard<std::mutex> lck(mutex_);
}
