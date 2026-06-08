#include "engine.h"

Engine * Engine::singleton() {
	static Engine instance;
	return &instance;
}

void Engine::workLazyTasks() {
	//for (std::size_t i = 0; i < lazy_tasks_.size(); i++) {
	if (lazy_tasks_.empty())
		return;
	
	std::packaged_task<void()> task;
	{
		std::lock_guard lock(lazy_tasks_mutex_);
		task = std::move(lazy_tasks_.front());
	}
	task();
	{
		std::lock_guard lock(lazy_tasks_mutex_);
		lazy_tasks_.pop();
	}
	//}
}
