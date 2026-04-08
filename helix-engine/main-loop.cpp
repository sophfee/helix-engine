#include "main-loop.hpp"

std::unique_ptr<IMainLoop> Main::main_loop_ = nullptr;

Error Main::start(std::unique_ptr<IMainLoop> main_loop) {
	Error result = OK;
	
	if (main_loop_) _UNLIKELY
		result = main_loop_->stop();
	
	if (result != OK) _UNLIKELY
		return result;

	main_loop_ = std::move(main_loop);
		result = main_loop_->start();

	return result;
}

Error Main::iter() {
	if (!main_loop_) _UNLIKELY
		return FAILED;
	
	Error const result = main_loop_->iter();
	return result;
}

Error Main::stop() {
	main_loop_->stop();
	return OK;
}