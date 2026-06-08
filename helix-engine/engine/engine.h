#pragma once
#include <future>

#include "types.hpp"

class Engine {
	std::queue<_STD packaged_task<void()>> lazy_tasks_;
	std::mutex lazy_tasks_mutex_;
	
	u64 frame_count = 0;
	
public:
	static Engine* singleton();

	template <typename R, typename ...Args>
	std::future<R> addLazyTaskToMainThreadQueue(_STD function<R(Args...)> func, Args...args) {
		_STD packaged_task<void()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push(std::move(task));
		return lazy_tasks_.back().get_future();
	}

	template <typename R, typename ...Args>
	std::future<R> addLazyTaskToMainThreadQueue(R(func)(Args...), Args...args) {
		_STD packaged_task<void()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push(std::move(task));
		return lazy_tasks_.back().get_future();
	}

	template <typename R, typename T, typename ...Args>
	std::future<R> addLazyTaskToMainThreadQueue(_STD function<R(T::*)(Args...)> func, Args...args) {
		_STD packaged_task<void()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push(std::move(task));
		return lazy_tasks_.back().get_future();
	}

	template <typename R, typename T, typename ...Args>
	std::future<R> addLazyTaskToMainThreadQueue(R(T::*func)(Args...), Args...args) {
		_STD packaged_task<void()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push(std::move(task));
		return lazy_tasks_.back().get_future();
	}

	template <typename ...Args>
	std::future<void> addLazyTaskToMainThreadQueue(void(func)(Args...), Args...args) {
		_STD packaged_task<void()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push(std::move(task));
		return lazy_tasks_.back().get_future();
	}

	template <typename ...Args>
	std::future<void> addLazyTaskToMainThreadQueue(std::future<void(Args...)> func, Args...args) {
		_STD packaged_task<void()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push(std::move(task));
		return lazy_tasks_.back().get_future();
	}

	std::future<void> addLazyTaskToMainThreadQueue(auto func) {
			_STD packaged_task<void()> task(std::move(func));
			std::lock_guard lock(lazy_tasks_mutex_);
			lazy_tasks_.push(std::move(task));
		return lazy_tasks_.back().get_future();
	}

	void workLazyTasks();
};
