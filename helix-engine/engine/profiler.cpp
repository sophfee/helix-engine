#include "profiler.hpp"

#if PROFILER_ENABLED

using namespace profiler;


void Profiler::pushStage(CName name) {
	
}

Profiler &profiler::instance() {
	static Profiler instance;
	return instance;
}

#endif
