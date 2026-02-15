#pragma once

#include <memory>

#include "types.hpp"

class IMainLoop {
public:
	virtual ~IMainLoop() = default;
	virtual void start() = 0;
	virtual void iterate() = 0;
	virtual void stop() = 0;
	[[nodiscard]] virtual bool iterating() const = 0;
};

class CDefMainLoop : public IMainLoop {
public:
	void start() override;
	void iterate() override;
	void stop() override;
	[[nodiscard]] bool iterating() const override;
private:
	bool iterating_ = true;
};

class CMain final {
	static std::unique_ptr<IMainLoop> main_loop_;
	static u64 frames_;

public:
	static Error start(std::unique_ptr<IMainLoop> main_loop);
	static Error iterate();
	static Error stop();
	static u64 frames();
};