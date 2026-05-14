#pragma once

#include "types.hpp"
#include "ecs/core/entity.hpp"
#include "ecs/core/scene_tree.hpp"
#include "gpu/graphics.hpp"

class IResource : public NoCopy {
public:
	// Returns a unique identifier for this particular resource. This is sometimes based on the internal data and sometimes is pseudo-random.
	[[nodiscard]] virtual _STD string_view uid() const noexcept = 0;
	// Returns the number of places this resource is being used.
	[[nodiscard]] virtual u32 users() const noexcept = 0;
	virtual void incrementReference() noexcept = 0;
	virtual void decrementReference() noexcept = 0;
};

template <typename T>
class Ref {
private:
	using decayed_type = std::decay_t<T>;
	using reference_type = decayed_type&;
	using reference_const_type = decayed_type const&;
	using pointer_type = decayed_type*;
	using pointer_const_type = decayed_type const*;
	
	struct reference_state {
		T resource_ = nullptr;
		u16 users_ = 0;
	};
	mutable reference_state *state_ = nullptr;
public:
	template <typename ...Args>
	Ref(Args...args) noexcept;
	Ref(Ref const &reference) noexcept;
	Ref &operator=(Ref const &reference) noexcept;
	Ref(Ref &&reference) noexcept;
	Ref &operator=(Ref &&reference) noexcept;

	
	[[nodiscard]] pointer_const_type get() const noexcept {
		return (pointer_type)state_; // Offset 0
	}
	
	[[nodiscard]] pointer_type get() noexcept {
		return (pointer_type)state_; // Offset 0
	}
	
	[[nodiscard]] pointer_const_type operator->() const noexcept {
		return (pointer_const_type)state_;
	}

	[[nodiscard]] pointer_type operator->() noexcept {
		return (pointer_type)state_; // Offset 0
	}
	
	[[nodiscard]] pointer_const_type   operator*()  const noexcept {
		return (pointer_const_type)state_;
	}
	
	[[nodiscard]] pointer_type operator*()  noexcept {
		return (pointer_type)state_;
	}
	
	[[nodiscard]] u32  users() const noexcept;
	[[nodiscard]] bool valid() const noexcept;
	
	void incrementReference() noexcept;
	void decrementReference() noexcept;
	
};

template <typename T>
template <typename ... Args>
Ref<T>::Ref(Args... args) : state_(new reference_state{}) {
	state_->resource_ = std::construct_at(&state_->resource_, args...);
	state_->users_ = 1;
}

template <typename T>
Ref<T>::Ref(Ref const &reference) {
	state_ = reference.state_;
	++state_->users_;
}

template <typename T>
Ref<T> & Ref<T>::operator=(Ref const &reference) {
	if (std::addressof(reference) == this) return *this; // self assignment check & err prevention
	if (valid()) decrementReference();
	state_ = reference.state_;
	incrementReference(); // new reference
	return *this;
}

template <typename T>
Ref<T>::Ref(Ref &&reference) noexcept {
	state_ = reference.state_;
	reference.state_ = nullptr;
}

template <typename T>
Ref<T> &Ref<T>::operator=(Ref &&reference) noexcept {
	if (std::addressof(reference) == this) return *this;
	if (valid()) decrementReference();
	state_ = reference.state_;
	reference.state_ = nullptr;
	return *this;
}

template <typename T> u32 Ref<T>::users() const noexcept { return static_cast<u32>(this->state_->users_); }
template <typename T> bool Ref<T>::valid() const noexcept { return this->state_ != nullptr; }
template <typename T> void Ref<T>::incrementReference() noexcept { ++this->state_->users_; }
template <typename T> void Ref<T>::decrementReference() noexcept { --this->state_->users_;
	if (this->state_->users_ == 0)
		delete this->state_->resource_;
}