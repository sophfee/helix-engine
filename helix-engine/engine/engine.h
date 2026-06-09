#pragma once
#include <future>

#include "lazy_task.hpp"
#include "types.hpp"

class Engine {
	Deque<LazyTask<bool()>> lazy_tasks_;
	std::mutex lazy_tasks_mutex_;
	
	u64 frame_count = 0;
	inline static thread_local bool main_thread = false;
	
public:
	static Engine* singleton();

	template <typename R, typename ...Args>
	std::future<R> addLazyTaskToMainThreadQueue(_STD function<R(Args...)> func, Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename R, typename ...Args>
	std::future<R> addLazyTaskToMainThreadQueue(R(func)(Args...), Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename R, typename T, typename ...Args>
	std::future<R> addLazyTaskToMainThreadQueue(_STD function<R(T::*)(Args...)> func, Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename R, typename T, typename ...Args>
	std::future<R> addLazyTaskToMainThreadQueue(R(T::*func)(Args...), Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename ...Args>
	std::future<bool> addLazyTaskToMainThreadQueue(void(func)(Args...), Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename ...Args>
	std::future<bool> addLazyTaskToMainThreadQueue(std::future<void(Args...)> func, Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	std::future<bool> addLazyTaskToMainThreadQueue(auto func) {
		LazyTask<bool()> task(func);
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	void markAsMainThread();
	[[nodiscard]] bool isOnMainThread() const;

	void incrementFrameCount();
	[[nodiscard]] u64 frameCount() const;

	void workLazyTasks();
};
