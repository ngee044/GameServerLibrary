#pragma once
#include <stack>
#include <map>
#include <vector>

/*

*/

class DeadLockProfiler
{
public:
	void push_lock(const char* name);
	void pop_lock(const char* name);
	void check_cycle();

protected:
	void dfs(int32 index);

private:
	std::unordered_map<const char*, int32> name_told_;
	std::unordered_map<int32, const char*> id_to_name_;
	std::stack<int32> lock_stack_;
	std::map<int32, set<int32>> lock_history_;

	Mutex lock_;

	std::vector<int32> discovered_order_; //��尡 �߰ߵ� ������ ����ϴ� �迭
	int32 discovered_count_ = 0; //��尡 �߰ߵ� ����
	std::vector<bool> finished_; //dfs(i)�� ���� �Ǿ����� ����
	std::vector<int32> parent_;
};

