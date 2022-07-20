#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::push_lock(const char* name)
{
	LockGuard guard(lock_);

	int32 lock_id = 0;
	
	auto find_iter = name_told_.find(name);
	if (find_iter == name_told_.end())
	{
		lock_id = static_cast<int32>(name_told_.size());
		name_told_[name] = lock_id;
		id_to_name_[lock_id] = name;
	}
	else
	{
		lock_id = find_iter->second;
	}

	//��� �ִ� ���� �־��ٸ�?
	if (lock_stack_.empty() == false)
	{
		//������ �߰ߵ��� ���� ���̽���� ����� ���� �ٽ� Ȯ���Ѵ�.
		const int32 prev_id = lock_stack_.top();
		if (lock_id != prev_id)
		{
			std::set<int>& history = lock_history_[prev_id];
			if (history.find(lock_id) == history.end())
			{
				history.insert(lock_id);
				check_cycle();
			}
		}

	}

	lock_stack_.push(lock_id);
}

void DeadLockProfiler::pop_lock(const char* name)
{
	LockGuard guard(lock_);

	if (lock_stack_.empty())
	{
		CRASH("MULTIPLE_UNLOCK");
	}

	int32 lock_id = name_told_[name];
	if (lock_stack_.top() != lock_id)
	{
		CRASH("INVALID_NULOCK");
	}

	lock_stack_.pop();
}

void DeadLockProfiler::check_cycle()
{
	const int32 lock_count = static_cast<int32>(name_told_.size());
	discovered_order_ = std::vector<int32>(lock_count, -1);
	discovered_count_ = 0;

	finished_ = std::vector<bool>(lock_count, false);
	parent_ = std::vector<int32>(lock_count, -1);

	for (int32 lock_id = 0; lock_id < lock_count; lock_id++)
	{
		dfs(lock_id);
	}

	discovered_order_.clear();
	finished_.clear();
	parent_.clear();
}

void DeadLockProfiler::dfs(int32 here)
{
	if (discovered_order_[here] != -1)
	{
		return;
	}

	discovered_order_[here] = discovered_count_++;

	//��� ������ ������ ��ȸ�Ѵ�.
	auto find_iter = lock_history_.find(here);
	if (find_iter == lock_history_.end())
	{
		finished_[here] = true;
		return;
	}

	std::set<int32>& next_set = find_iter->second;
	for (int32 there : next_set)
	{
		//���� �湮�� ���� ���ٸ� �湮�Ѵ�.
		if (discovered_order_[there] == -1)
		{
			parent_[there] = here;
			dfs(there);
			continue;
		}

		//here�� there���� ���� �߰ߵǾ��ٸ�, there�� here�� �ļ��̴�. (������ ����)
		if (discovered_order_[here] < discovered_order_[there])
		{
			continue;
		}

		//�������� �ƴϰ�, DFS(there)�� ���� �������� �ʾҴٸ�,there�� here�� �����̴�.(������ ����)
		if (finished_[there] == false)
		{
			printf("%s -> %s \n", id_to_name_[here], id_to_name_[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s \n", id_to_name_[parent_[now]], id_to_name_[now]);
				now = parent_[now];
				if (now == there)
				{
					break;
				}
			}
			CRASH("DEADLOCK_DETECTED");
		}

	}

	finished_[here] = true;
}



