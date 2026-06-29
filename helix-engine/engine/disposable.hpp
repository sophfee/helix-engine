#pragma once

/**
 * A disposable resource is a resource that can be disposed (and sometimes, recomposed) at any point in its container (class') lifecycle, including prior to the object's deconstruction. 
 */
class IDisposable {
public:
	IDisposable() = default;
	IDisposable(IDisposable const &) = delete;
	IDisposable &operator=(IDisposable const &) = delete;
	virtual ~IDisposable() = default;
	virtual void dispose() = 0;
	virtual [[nodiscard]] bool disposed() const = 0;
};