#include "pch.h"
#include "UserManager.h"

void UserManager::ProcessSave()
{
	std::lock_guard<std::mutex> lck(mutex_);
}
