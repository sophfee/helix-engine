#include <iostream>
#include <ostream>
#include <string>

#include "os.hpp"
#include "engine/engine.h"
#include "engine/filesystem.hpp"
#include "engine/main-loop.hpp"
#include "engine/thread_pool.hpp"
#include "gpu/graphics.hpp"
#include "gpu/lighting.hpp"
#include "gpu/model_manager.hpp"
#include "gpu/render_server.h"
#include "gpu/texture.h"

#define USE_HIGH_RESOLUTION_CLOCK

#ifdef USE_HIGH_RESOLUTION_CLOCK
using clock_type = std::chrono::high_resolution_clock;
#else
using clock_type = std::chrono::steady_clock;
#endif

int main(int argc, char **argv) {
	try {
		using std::chrono::high_resolution_clock;

		Engine::singleton()->markAsMainThread();

		/* Preinitialize our graphics */
		initGraphics();
	
		std::string startup_scene = "test-resources\\sponza\\NewSponza_Main_glTF_003.gltf";

		if (argc == 2)
			startup_scene = argv[1];

	
		UniquePtr<IMainLoop> main_loop = std::make_unique<DefMainLoop>();
		Result<> result = Main::start(
			std::move(main_loop),
			startup_scene
		);
	
		if (result.error() != OK) {
			std::cerr << "Failed to start main loop: " << result.error() << '\n';
			return -1;
		}

		/* The main loop must make its context current! */
		/* All singletons are now established here, */
		AsyncTextureBank *async_texture_bank = AsyncTextureBank::singleton();
		LightingSystem *lighting_system = LightingSystem::singleton();
		ModelManager *model_manager = ModelManager::singleton();
		ThreadPool *thread_pool = ThreadPool::singleton();
		FileSystem::singleton();
		RenderServer::singleton(); //< TODO: Needs to be gutted. Didn't know exactly what I wanted with this and turned out to be a bigger mess than it was worth.

		//< TODO: Is this even functional? I believe I moved all file watching to the Shader Programs themselves, but that should also be changed.
		os::initDirectoryWatcher();

		clock_type::time_point start_time = clock_type::now();
		Result is_running = true;
		while (result.error() == OK && is_running.has_value() && is_running.value()) {
			Engine::singleton()->workLazyTasks();
			clock_type::duration delta = clock_type::now() - start_time;
			is_running = Main::running();
			start_time = clock_type::now();
			if (is_running.has_value() && !is_running.value())
				break;
			result = Main::iter(std::chrono::duration_cast<std::chrono::duration<f64>>(delta).count());
		}
	
		result = Main::stop();
		if (result.error() != OK) {
			std::cerr << "Failed to stop main loop: " << result.error() << '\n';
			return -1;
		}
		async_texture_bank->dispose();
		lighting_system->dispose();
		model_manager->dispose();

		terminateGraphics();

		return 0;
	}
	catch (std::exception const &e) {
		MessageBoxA(nullptr, e.what(), nullptr, MB_OK);
		return -1;
	}
}
