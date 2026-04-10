#pragma once

class IDisposable {
public:
	IDisposable(IDisposable const &) = delete;
	IDisposable() = default;
	IDisposable &operator=(IDisposable const &) = delete;
	virtual ~IDisposable() = default;
	virtual void dispose() = 0;
	virtual [[nodiscard]] bool disposed() const = 0;
};