#pragma once
#include <future>

#include "lazy_task.hpp"
#include "types.hpp"
#include "gpu/gl_ext.hpp"
#define EXTENSION_SUPPORTED(EXTENSION_NAME) (Engine::singleton()->supportedExtensions().##EXTENSION_NAME##_supported)
#define EXTENSION_NOT_SUPPORTED(EXTENSION_NAME) (!Engine::singleton()->supportedExtensions().##EXTENSION_NAME##_supported)

class Engine {
	Deque<LazyTask<bool()>> lazy_tasks_;
	std::mutex lazy_tasks_mutex_;
	
	u64 frame_count = 0;
	inline static thread_local bool main_thread = false;
	
public:
	static Engine* get_singleton();

	template <typename R, typename ...Args>
	std::future<R> add_lazy_task_to_main_thread_queue(_STD function<R(Args...)> func, Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename R, typename ...Args>
	std::future<R> add_lazy_task_to_main_thread_queue(R(func)(Args...), Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename R, typename T, typename ...Args>
	std::future<R> add_lazy_task_to_main_thread_queue(_STD function<R(T::*)(Args...)> func, Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename R, typename T, typename ...Args>
	std::future<R> add_lazy_task_to_main_thread_queue(R(T::*func)(Args...), Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename ...Args>
	std::future<bool> add_lazy_task_to_main_thread_queue(void(func)(Args...), Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	template <typename ...Args>
	std::future<bool> add_lazy_task_to_main_thread_queue(std::future<void(Args...)> func, Args...args) {
		LazyTask<bool()> task(std::bind(func, args...));
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	std::future<bool> add_lazy_task_to_main_thread_queue(auto func) {
		LazyTask<bool()> task(func);
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.push_back(std::move(task));
		return lazy_tasks_.back().future();
	}

	void set_as_main_thread();
	[[nodiscard]] bool is_on_main_thread() const;

	void increment_frame_count();
	[[nodiscard]] u64 get_frame_count() const;

	void work_lazy_tasks();

	Extensions &get_supported_gl_extensions();
};
