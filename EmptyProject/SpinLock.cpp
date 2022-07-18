#include "SpinLock.h"

#include <atomic>
#include <mutex>
#include <thread>

//SPIN LOCK

//Lock�� ���� �� ���ٸ�, ����ؼ� Lock�� Ȯ���ϸ� ���� ������ ��ٸ���.
//�̸��� �ٻڰ� ��ٸ��� busy waiting�̴�.

//�ٻڰ� ��ٸ��ٴ� ���� ���� ������ ���鼭 �ִ��� �ٸ� �����忡�� CPU�� �纸���� �ʴ� ���̴�.

//Lock�� �� ��밡������ ��� Context Switching�� �ٿ� CPU�� �δ��� �����ش�.
//������, ���� � �����尡 Lock�� �������� �����Ѵٸ� ������ CPU �ð��� ���� �Ҹ��� ���ɼ��� �ִ�.
//�ϳ��� CPU�� �ϳ��� �ھ �ִ� ��쿡�� �������� �ʴ�.
//�� ������ ���� �ٸ� �����尡 Lock�� ������ �ְ� �� �����尡 Lock�� Ǯ�� �ַ��� �̱� CPU ���� 100 % �� ����� ��Ȳ�� �߻��ϹǷ� �����ؾ��Ѵ�.
//���ɶ��� �⺻������ ���� for������ ���鼭 lock�� ��ٸ��Ƿ� �ϳ��� �����尡 lock�� �������� ������ �ִٸ�, �ٸ� Blocking�� ������� busy waiting�� �ϹǷ� CPU�� �������� �����ϰ� �ȴ�.

//������ Spin Lock�� �� ����ϸ� context switch�� �ٿ� ȿ���� ���� �� �ֽ��ϴ�.

//���� ������ ���� ���ٴ� ���� �ð� lock�� ���� �� ���ٸ� ��� sleep�ϴ� back off �˰����� ����ϴ°��� �ξ� �����ϴ�.

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
			
			���� ��� ���ɶ��� 5000�� ���� �� �ȵǴ� ��Ȳ�� �� ��� time signal�� ������ 
			count_limits_++;
			if(count_limits_ == 5000)
			{
			...
			}

			�ΰ��� ���
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

