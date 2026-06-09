// ReSharper disable CppClangTidyClangDiagnosticReservedIdentifier
// ReSharper disable CppClangTidyBugproneReservedIdentifier
#pragma once
#include <assert.h>
#include <future>

template <class>
class LazyTask; // not defined

template <class Ret, class... ArgTypes>
class LazyTask<Ret(ArgTypes...)> {
	using Fty = Ret(ArgTypes...);
public:
	LazyTask() = default;

	template <
		class Fty2,
		typename = std::enable_if_t<
			!std::is_same_v<std::decay_t<Fty2>, Fty>
		>
	>
	explicit LazyTask(Fty2 &&Fnarg) : func_(Fnarg), promise_() { assert((bool)func_); }

	LazyTask(LazyTask const &) = delete;
	LazyTask& operator=(LazyTask const &) = delete;

	LazyTask(LazyTask &&) = default;
	LazyTask& operator=(LazyTask &&) = default;

	~LazyTask() {
	}

	Ret operator()(ArgTypes... args) {
		assert((bool)func_);
		Ret ret_val = func_(std::forward<ArgTypes>(args)...);
		if (ret_val)
			promise_.set_value(std::move(ret_val));
		return ret_val;
	}

	_NODISCARD_GET_FUTURE _STD future<Ret> future() {
		return promise_.get_future();
	}
	
private:

	_STD function<Fty> func_;
	_STD promise<Ret>  promise_;
};