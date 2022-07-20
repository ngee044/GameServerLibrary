#include <thread>
#include <mutex>
#include <queue>
#include <iostream>
#include <Windows.h>

std::mutex m;
std::queue<int> q;
HANDLE handle;

//커널 오브젝트는 아니고 (User-Level)
std::condition_variable cv;

void Producer()
{
	while (true)
	{
		std::unique_lock<std::mutex> lck(m);
		q.push(100);
	}

	//Windows API는 커널 오브젝트이다
	::SetEvent(handle);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, (DWORD)INFINITY);

		std::unique_lock<std::mutex> lck(m);
		if (!q.empty())
		{
			auto data = q.front();
			q.pop();
			std::cout << data << std::endl;
		}
	}
}
//
//int main()
//{
//	handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
//
//	std::thread t1(Producer);
//	std::thread t2(Consumer);
//
//	t1.join();
//	t2.join();
//
//	::CloseHandle(handle);
//}