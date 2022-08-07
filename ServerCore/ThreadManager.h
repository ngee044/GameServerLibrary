#pragma once

#include <thread>
#include <functional>

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void launch(std::function<void(void)> callback);
	void join();
	
	//tls is thread local storage
	static void init_tls();
	static void destroy_tls();

private:
	Mutex lock_;
	vector<std::thread> threads_;

};

