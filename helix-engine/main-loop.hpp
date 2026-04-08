#pragma once

#include <memory>

#include "types.hpp"

class IMainLoop {
public:
	virtual ~IMainLoop() = default;

	virtual Error start() = 0;
	virtual Error iter() = 0;
	virtual Error stop() = 0;
};

class Main final {
	static UniquePtr<IMainLoop> main_loop_;
	
public:
	static Error start(std::unique_ptr<IMainLoop> main_loop);
	static Error iter();
	static Error stop();
};