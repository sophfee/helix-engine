#include "main-loop.hpp"

std::unique_ptr<IMainLoop> CMain::main_loop_ = nullptr;
u64 CMain::frames_ = 0;

void CDefMainLoop::start() {
	
}

void CDefMainLoop::iterate() {}

void CDefMainLoop::stop() {}

bool CDefMainLoop::iterating() const { return iterating_; }

Error CMain::start(std::unique_ptr<IMainLoop> main_loop) {
	if (main_loop_) _UNLIKELY
		main_loop_->stop();
	main_loop_ = std::move(main_loop);
	main_loop_->start();
	return OK;
}

Error CMain::iterate() {
	if (!main_loop_->iterating()) _UNLIKELY
		return FAILED;
	main_loop_->iterate();
	frames_++;
	return OK;
}

Error CMain::stop() {
	main_loop_->stop();
	return OK;
}

u64 CMain::frames() {
	return frames_;
}