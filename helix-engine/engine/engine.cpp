#include "engine.h"

#include "gpu/png.hpp"

Engine * Engine::singleton() {
	static Engine instance;
	return &instance;
}

void Engine::markAsMainThread() {
	main_thread = true;
}
bool Engine::isOnMainThread() const {
	return main_thread;
}

void Engine::incrementFrameCount() {
	frame_count++;
}
u64 Engine::frameCount() const {
	return frame_count;
}

void Engine::workLazyTasks() {
	//for (std::size_t i = 0; i < lazy_tasks_.size(); i++) {
	if (lazy_tasks_.empty())
		return;

	for (size_t i = 0; i < lazy_tasks_.size(); i++) {
		LazyTask<bool()> *task;
		{
			std::lock_guard lock(lazy_tasks_mutex_);
			task = &lazy_tasks_[i];
		}
		
		if ((*task)()) {
			std::lock_guard lock(lazy_tasks_mutex_);
			lazy_tasks_.erase(lazy_tasks_.begin() + i);
		}
	}
	//}
}
