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

	//잡고 있는 락이 있었다면?
	if (lock_stack_.empty() == false)
	{
		//기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인한다.
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

	//모든 인접한 정점을 순회한다.
	auto find_iter = lock_history_.find(here);
	if (find_iter == lock_history_.end())
	{
		finished_[here] = true;
		return;
	}

	std::set<int32>& next_set = find_iter->second;
	for (int32 there : next_set)
	{
		//아직 방문한 적이 없다면 방문한다.
		if (discovered_order_[there] == -1)
		{
			parent_[there] = here;
			dfs(there);
			continue;
		}

		//here가 there보다 먼저 발견되었다면, there는 here의 후손이다. (순방향 간선)
		if (discovered_order_[here] < discovered_order_[there])
		{
			continue;
		}

		//순방향이 아니고, DFS(there)가 아직 종료하지 않았다면,there는 here의 선조이다.(역방향 간선)
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



