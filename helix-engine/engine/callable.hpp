#pragma once
#include <stdexcept>
#include <type_traits>

class CallBehavior {
public:
	virtual ~CallBehavior() {}
	virtual void callp(void** argv, size_t argc) = 0;
	virtual void callpr(void ** argv, size_t argc, void *outv) = 0;
};

template <typename TRet, typename... TArgs>
class CallBehaviorStatic : public CallBehavior {
	
public:

	using FunctionType = TRet(TArgs...);
	FunctionType *func;

	CallBehaviorStatic(FunctionType *p_func) : func(p_func) {}
	
	
	void callp(void** argv, size_t const argc) override {
		if (argc != sizeof...(TArgs))
			throw std::runtime_error("Argument count mismatch");

		func(*static_cast<TArgs*>(argv[0])...);
	}

	void callpr(void ** argv, size_t const argc, void *outv) override {
		if (argc != sizeof...(TArgs))
			throw std::runtime_error("Argument count mismatch");

		TRet *ret = (TRet*)outv;
		*ret = func(*static_cast<TArgs*>(argv[0])...);
	}
};

template <typename TRet, typename TCls, typename... TArgs>
class CallBehaviorMethod : public CallBehavior {
public:
	using FunctionType = TRet(TCls::*)(TArgs...);
	FunctionType func;
	CallBehaviorMethod(FunctionType *p_func) : func(p_func) {}
	void callp(void** argv, size_t const argc) override {
		if (argc - 1 != sizeof...(TArgs)) // extra pad for 'this' parameter
			throw std::runtime_error("Argument count mismatch");

		TCls *obj = static_cast<TCls*>(argv[0]);
		(obj->*func)(*static_cast<TArgs*>(argv[1])...);
	}

	void callpr(void ** argv, size_t const argc, void *outv) override {
		if (argc - 1 != sizeof...(TArgs) && argc != sizeof...(TArgs))
			throw std::runtime_error("Argument count mismatch");
		TCls *obj = static_cast<TCls*>(argv[0]);
		TRet *ret = (TRet*)outv;
		*ret = (obj->*func)(*static_cast<TArgs*>(argv[1])...);
	}
};

class Callable {
	CallBehavior *behavior_;
public:

	template <typename TRet, typename... TArg>
	Callable(TRet(*func)(TArg...)) : behavior_(new CallBehaviorStatic<TRet, TArg...>(func)) {}

	template <typename TRet, typename TCls, typename... TArgs>
	Callable(TRet(TCls::*func)(TArgs...)) : behavior_(new CallBehaviorMethod<TRet, TCls, TArgs...>(func)) {}

	~Callable() { delete behavior_; }

	template <typename TRet, typename... TArgs>
	TRet call(TArgs... args) {
		void* argv[] = { &args... };
		TRet ret;
		behavior_->callpr(argv, sizeof...(TArgs), &ret);
		return ret;
	}

	void callpr(void ** argv, size_t const argc, void *outv) const {
		behavior_->callpr(argv, argc, outv);
	}
};