#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t const thread_count) : threads_(thread_count), sem_(0), stop_(false) {
	for (size_t i = 0; i < thread_count; ++i)
		threads_.emplace_back([this, i] { Worker(i); });
	
}

void ThreadPool::Worker(size_t thread_index) {
	while (true) {
		while (!stop_.load()) {
			if (sem_.try_acquire())
				break;
		}

		Task<void()> task;
		{
			std::lock_guard lock(queue_mutex_);
			if (queue_.empty())
				break;
			task = std::move(queue_.front());
			queue_.pop();
		}
		task();
	}
}

ThreadPool::~ThreadPool() {
	//
	// Threads are JThreads, so they will be joined automatically, however they will be at a deadlock if not enough Semaphore ticks are set
	//
	stop_.store(true, std::memory_order::seq_cst);
	sem_.release(static_cast<std::ptrdiff_t>(threads_.size()));
}

ThreadPool * ThreadPool::singleton() {
	static ThreadPool singleton_;
	return &singleton_;
}
