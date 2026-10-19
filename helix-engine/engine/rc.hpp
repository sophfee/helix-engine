#pragma once
#include "object.hpp"

class IRefCounted {
public:
	virtual ~IRefCounted() = 0;
	virtual void increment_reference() = 0;
	virtual void decrement_reference() = 0;
	[[nodiscard]] virtual u32 get_reference_count() const = 0;
	[[nodiscard]] virtual bool should_free() const = 0;
};

class RefCounted : public Object, public IRefCounted {
public:
	void increment_reference() override;
	void decrement_reference() override;
	[[nodiscard]] u32 get_reference_count() const override;
	[[nodiscard]] bool should_free() const override;
protected:
	void set_auto_free(const bool auto_free) { auto_free_ = auto_free; }
	[[nodiscard]] bool is_auto_free() const { return auto_free_; }
private:
	u16 ref_count_ = 0;
	bool auto_free_ = true;
public:
	Any get(const StringView property) const override;
};

class AtomicRefCounted : public Object {
public:
	void increment_reference();
	void decrement_reference();
	[[nodiscard]] u32 get_reference_count() const;
	[[nodiscard]] bool should_free() const;
protected:
	void set_auto_free(const bool auto_free) { auto_free_ = auto_free; }
	[[nodiscard]] bool is_auto_free() const { return auto_free_; }
private:
	std::atomic<u32> ref_count_ = 0;
	bool auto_free_ = true;
public:
	Any get(const StringView property) const override;
};

template <typename T> requires (std::is_base_of_v<IRefCounted, T>)
class Ref {
public:
	Ref() = default;
	Ref(T* ptr) : ptr_(ptr) {
		if (ptr_) ptr_->increment_reference();
	}
	Ref(const Ref& other) : ptr_(other.ptr_) {
		if (ptr_) ptr_->increment_reference();
	}
	Ref(Ref&& other) noexcept : ptr_(other.ptr_) {
		other.ptr_ = nullptr;
	}
	~Ref() {
		if (ptr_) ptr_->decrement_reference();
	}

	Ref& operator=(const Ref& other) {
		if (this != &other) {
			if (ptr_) ptr_->decrement_reference();
			ptr_ = other.ptr_;
			if (ptr_) ptr_->increment_reference();
		}
		return *this;
	}

	Ref& operator=(Ref&& other) noexcept {
		if (this != &other) {
			if (ptr_) ptr_->decrement_reference();
			ptr_ = other.ptr_;
			other.ptr_ = nullptr;
		}
		return *this;
	}
	
	T* get() const { return ptr_; }

	T* operator->() { return ptr_; }
	const T* operator->() const { return ptr_; }

	T& operator*() { return *ptr_; }
	const T& operator*() const { return *ptr_; }
	
	explicit operator bool() const { return ptr_ != nullptr; }
	
private:
	T* ptr_ = nullptr;
};