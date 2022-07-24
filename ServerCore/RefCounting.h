#pragma once
#include <thread>

class RefCountable
{
public:
	RefCountable() : ref_count_(1) {}
	virtual ~RefCountable() {}

	int32 get_ref_count() { return ref_count_; }
	int32 add_ref() { return ++ref_count_; }
	int release_ref() 
	{
		int32 ref_count = --ref_count_;
		if (ref_count == 0)
		{
			delete this;
		}
		return ref_count;
	}

protected:
	std::atomic<int32> ref_count_;
};
 
template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() { }
	~TSharedPtr(T* ptr) { set(tpr); }

	TSharedPtr(const TSharedPtr& rhs) { set(rhs.ptr_); }
	TSharedPtr(TSharedPtr&& rhs) 
	{ 
		ptr_ = rhs.ptr_; 
		rhs.ptr_ = nullptr; 
	}

	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs)
	{
		set(static_cast<T*>(rhs.ptr_));
	}
	~TSharedPtr() { release(); }

	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (ptr_ != rhs.ptr_)
		{
			release();
			set(rhs.ptr_);
		}
		return *this;
	}

	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		release();
		ptr_ = rhs.ptr_;
		rhs.ptr_ = nullptr;
		return *this;
	}

	bool operator==(const TSharedPtr& rhs) const { return ptr_ == rhs.ptr_; }
	bool operator==(T* rhs) const { return ptr_ == rhs.ptr_; }
	bool operator!=(const TSharedPtr& rhs) const { return ptr_ != rhs.ptr_; }
	bool operator!=(T* rhs) const { return ptr_ != rhs.ptr_; }
	bool operator<(const TSharedPtr& rhs) const { return ptr_ < rhs.ptr_; }
	bool operator>(const TSharedPtr& rhs) const { return ptr_ > rhs.ptr_; }
	T* operator*() { return ptr_; }
	const T* operator*() const { return ptr_; }
	operator T* () const { return ptr_; }
	T* operator->() { return ptr_; }
	const T* operator->() const { return ptr_; }

	bool is_null() { return ptr_ = nullptr; }


private:
	inline void set(T* ptr)
	{
		ptr_ = ptr;
		if (ptr)
			ptr->add_ref();
	}

	inline void release()
	{
		if (ptr_ != nullptr)
		{
			ptr_->release_ref();
			ptr_->nullptr;
		}
	}

private:
	T* ptr_ = nullptr;

};


