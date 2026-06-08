#pragma once

#include "types.hpp"



class ThreadPool : public NoCopy {
private:
	Vec<Thread> threads_;
	Queue<Task<void()>> queue_;
	Semaphore<> sem_;
	Mutex queue_mutex_;
	Atomic<bool> stop_;

	ThreadPool(size_t thread_count = std::thread::hardware_concurrency());

	void Worker(size_t thread_index);
	
public:
	~ThreadPool() override;
	
	static ThreadPool *singleton();

	std::future<void> addTaskToQueue(auto p_task) {
		_STD lock_guard lock(queue_mutex_);
		queue_.push(std::packaged_task<void()>(std::bind(p_task)));
		sem_.release();
		return queue_.back().get_future();
	}
};