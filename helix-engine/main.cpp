#include <iostream>
#include <ostream>
#include <string>

//#define HELIX_TRY_CATCH_ON_WHOLE_APP

#include "os.hpp"
#include "engine/engine.h"
#include "engine/filesystem.hpp"
#include "engine/main-loop.hpp"
#include "engine/thread_pool.hpp"
#include "gpu/graphics.hpp"
#include "gpu/lighting.hpp"

#pragma comment(lib, "C:\\Program Files\\KTX-Software\\lib\\ktx.lib")

#define USE_HIGH_RESOLUTION_CLOCK

#ifdef USE_HIGH_RESOLUTION_CLOCK
using clock_type = std::chrono::high_resolution_clock;
#else
using clock_type = std::chrono::steady_clock;
#endif

int main(int argc, char **argv) {
#ifdef HELIX_TRY_CATCH_ON_WHOLE_APP
	try {
#endif
		using std::chrono::high_resolution_clock;

		Engine::get_singleton()->set_as_main_thread();

		/* Preinitialize our graphics */
		initialize_graphics();
	
		String startup_scene = "test-resources\\sponza\\NewSponza_Main_glTF_003.gltf";
		//	String startup_scene = "test-resources\\damaged_helmet\\damaged_helmet.gltf";
		//	String startup_scene = "test-resources\\NormalTangent.gltf";
		
		if (argc == 2) startup_scene = argv[1];
		
		UniquePtr<IMainLoop> main_loop = std::make_unique<DefMainLoop>();
		Result<> result = Main::start(std::move(main_loop), startup_scene);
	
		if (result.error() != OK) {
			std::cerr << "Failed to start main loop: " << result.error() << '\n';
			return -1;
		}
		/* The main loop must make its context current! */
		/* All singletons are now established here, */
		ThreadPool *thread_pool = ThreadPool::singleton();
		FileSystem::get_singleton();

		//< TODO: Is this even functional? I believe I moved all file watching to the Shader Programs themselves, but that should also be changed.
		os::init_directory_watcher();

		clock_type::time_point start_time = clock_type::now();
		Result is_running = true;
		while (result.error() == OK && is_running.has_value() && is_running.value()) {
			Engine::get_singleton()->work_lazy_tasks();
			clock_type::duration delta = clock_type::now() - start_time;
			is_running = Main::is_running();
			start_time = clock_type::now();
			if (is_running.has_value() && !is_running.value()) break;
			result = Main::iterate(std::chrono::duration_cast<std::chrono::duration<f64>>(delta).count());
		}
		LightingSystem *lighting_system = LightingSystem::singleton();
		lighting_system->dispose();
		
		result = Main::stop();
		if (result.error() != OK) {
			std::cerr << "Failed to stop main loop: " << result.error() << '\n';
			return -1;
		}

		shutdown_graphics();

		return 0;
#ifdef HELIX_TRY_CATCH_ON_WHOLE_APP
	}
	catch (std::exception const &e) {
		MessageBoxA(nullptr, e.what(), nullptr, MB_DEFBUTTON1);
		return -1;
	}
#endif
}
