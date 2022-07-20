#include <thread>
#include <vector>
#include <iostream>

bool isPrime(int num)
{
	if (num == 1)
		return false;

	if (num == 2 || num == 3)
		return true;

	for (auto i = 2; i < num; ++i)
	{
		if (i % num == 0)
			return false;
	}
	return true;
}

int get_prime_count(int start, int end)
{
	int cnt = 0;

	for (auto i = start; i <= end; ++i)
	{
		if (isPrime(i))
			cnt++;
	}
	return cnt;
}


int main()
{
	const int MAX_NUMBER = 1'000'000;

	std::vector<std::thread> threads;

	int core_count = std::thread::hardware_concurrency();
	int job_count = (MAX_NUMBER / core_count) + 1;

	std::atomic<int> prime_count = 0;

	for (auto i = 0; i < core_count; ++i)
	{
		int start = (i * job_count) + 1;
		int end = std::min(MAX_NUMBER, ((i + 1) * job_count));

		threads.push_back(std::thread([start, end, &prime_count](){
			prime_count += get_prime_count(start, end);
			}));
	}

	for (auto& t : threads)
	{
		t.join();
	}

	std::cout << prime_count << std::endl;

	return 0; 
}