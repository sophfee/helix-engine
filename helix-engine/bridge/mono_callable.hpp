#pragma once

#include "clr/coreclr_delegates.h"
#include "engine/callable.hpp"

template <typename TRet, typename... Args>
class MonoCallableBehavior {
	using mono_fn = TRet(CORECLR_DELEGATE_CALLTYPE*)(Args...);
	
};