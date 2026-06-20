#pragma once

#define PROFILER_ENABLED 1

//
// Some notes:
// Profiler should be built around macros so that it can be completely and wholely disabled with a simple preprocessor definition
// String pooling 
//

#if PROFILER_ENABLED

#include <chrono>

#include "name.hpp"

/* special namespace because this needs a bunch of isolated types that i just dont want clogging up the general space. */
namespace profiler {
	using namespace std::chrono;

	using TimeStamp = system_clock::time_point;

	constexpr auto MAX_SUB_STAGES = 100;

	struct Stage {
		TimeStamp start = system_clock::now();
		TimeStamp end;
		CName name;
		bool finished;

		void stop() {
			finished = true;
			end = system_clock::now();
		}
	};

	struct Frame {
		TimeStamp start;
		TimeStamp end;
	};

	class Profiler {
	public:

		void startFrame();
		void endFrame();

		void pushStage(CName name);
		void popStage();
		
	private:
		Array<Stage, MAX_SUB_STAGES> stages;
		std::size_t currentEntry;
		Frame frame;
	};

	static Profiler &instance();
}

#define ProfilerStartFrame()

#define ProfilerPushStage(STAGE_NAME)
#define ProfilerPopStage()

#define ProfilerEndFrame()

#else

#endif