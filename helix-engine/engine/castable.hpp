#pragma once

template <typename T>
class ICastable {
public:
	virtual ~ICastable() = default;
	[[nodiscard]] virtual const T cast() const = 0;
	[[nodiscard]] virtual T cast() = 0;
	[[nodiscard]] virtual bool cast_into(T& out) const = 0;
};