#include "SpinLock.h"

#include <atomic>
#include <mutex>
#include <thread>

//SPIN LOCK

//Lock을 얻을 수 없다면, 계속해서 Lock을 확인하며 얻을 때까지 기다린다.
//이른바 바쁘게 기다리는 busy waiting이다.

//바쁘게 기다린다는 것은 무한 루프를 돌면서 최대한 다른 스레드에게 CPU를 양보하지 않는 것이다.

//Lock이 곧 사용가능해질 경우 Context Switching을 줄여 CPU의 부담을 덜어준다.
//하지만, 만약 어떤 스레드가 Lock을 오랫동안 유지한다면 오히려 CPU 시간을 많이 소모할 가능성이 있다.
//하나의 CPU나 하나의 코어만 있는 경우에는 유용하지 않다.
//그 이유는 만약 다른 스레드가 Lock을 가지고 있고 그 스레드가 Lock을 풀어 주려면 싱글 CPU 사용률 100 % 를 만드는 상황이 발생하므로 주의해야한다.
//스핀락은 기본적으로 무한 for루프를 돌면서 lock을 기다리므로 하나의 스레드가 lock을 오랫동안 가지고 있다면, 다른 Blocking된 스레드는 busy waiting을 하므로 CPU를 쓸데없이 낭비하게 된다.

//장점은 Spin Lock을 잘 사용하면 context switch를 줄여 효율을 높일 수 있습니다.

//무한 루프를 돌기 보다는 일정 시간 lock을 얻을 수 없다면 잠시 sleep하는 back off 알고리즘을 사용하는것이 훨씬 좋습니다.

class SpinLock
{
public:
	void lock()
	{
		// CAS (Compare-And-Swap)
		bool expected = false;
		bool desired = true;

		while (locked_.compare_exchange_strong(expected, desired) == false)
		{
			expected = false;

			/*
			
			예를 들어 스핀락을 5000번 돌고 정 안되는 상황이 올 경우 time signal을 보낸다 
			count_limits_++;
			if(count_limits_ == 5000)
			{
			...
			}

			두가지 방법
			1)
			std::this_thread::yield();
			2)
			std::this_thread::sleep_for(0ms);
			
			*/
		}

	}

	void unlock()
	{
		locked_.store(false);
	}

private:
	std::atomic<int> count_limits_ = 0;
	std::atomic<bool> locked_ = false;
};

