#include "main-loop.hpp"

#include <fstream>

#include "util.hpp"
#include "ecs/ecs_gltf.hpp"
#include "ecs/3d/editor/editor_camera.hpp"
#include "ecs/core/scene_tree.hpp"
#include "gpu/driver.hpp"
#include "gpu/gltf.h"
#include "gpu/graphics.hpp"
#include "gpu/driver.hpp"
#include "gpu/window.hpp"
#include "gpu/renderers/forward.hpp"
#include "inipp/inipp.h"
#include "simdjson/simdjson.h"

std::unique_ptr<IMainLoop> Main::main_loop_ = nullptr;

Result<> Main::start(UniquePtr<IMainLoop> &&main_loop, std::string const &startup_scene) {
	Result<void> result = OK;
	
	if (main_loop_) _UNLIKELY
		result = main_loop_->stop();
	
	if (result.error() != OK) _UNLIKELY
		return result;

	main_loop_ = std::move(main_loop);
	result = main_loop_->start(startup_scene);

	return result;
}

Result<> Main::iter(f64 const delta) {
	if (!main_loop_) _UNLIKELY
		return FAILED;
	
	Result<> const result = main_loop_->iter(delta);
	return result;
}

Result<> Main::stop() {
	return main_loop_->stop();
}

Result<bool> Main::running() {
	return main_loop_->running();
}

Result<IRenderer *> Main::renderer() {
	return main_loop_->renderer();
}

Result<IMainLoop &> Main::mainLoop() {
	if (!main_loop_) _UNLIKELY
		return FAILED;
	return *main_loop_;
}

namespace {
	gltf::data loadModelAsync(std::string const &path) {
		auto gltf_path = simdjson::padded_string::load(path).value();
		return gltf::parse(path, std::move(gltf_path));
	}
}

Result<> DefMainLoop::start(std::string const &startup_scene) {
	std::ifstream config_stream("config.ini");
	config_.parse(config_stream);

	std::string renderer_name;

	const auto &sec_engine_graphics = config_.sections["Engine/Graphics"];
	const auto &sec_engine_graphics_window = config_.sections["Engine/Graphics/Window"];
	
	WindowDriver windowing = WindowDriver::eSdl2;
	std::string window_driver;
	inipp::get_value(sec_engine_graphics_window,
		"WindowingDriver", window_driver);
	
	switch (hash(window_driver)) {
		case hash("GLFW"):
		case hash("GLFW3"): {
			windowing = WindowDriver::eGlfw3;
			assert(glfwInit() == GLFW_TRUE && "GLFW failed to initialize");
			break;
		}
		case hash("SDL"):
		case hash("SDL2"): {
			windowing = WindowDriver::eSdl2;
			assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0 && "Failed to initialize SDL2");
			break;
		}
	default:
		printf("No windowing api specified... using SDL2.\n");
		break;
	}
	
	inipp::get_value(sec_engine_graphics,
		"Renderer", renderer_name);

	ivec2 window_size;
	inipp::get_value(sec_engine_graphics_window,
		"Width", window_size.x);
	inipp::get_value(sec_engine_graphics_window,
		"Height", window_size.y);

	std::string window_name;
	inipp::get_value(sec_engine_graphics_window,
		"Title", window_name);

	bool fullscreen;
	inipp::get_value(sec_engine_graphics_window,
		"Fullscreen", fullscreen);
	
	GraphicsDriver* driver = GraphicsDriver::singleton();
	driver->set_backend(RenderingApiBackend::eVulkan);
	
	window_ = std::make_shared<Window>(windowing, RenderingApiBackend::eVulkan, window_size, window_name, std::nullopt, WindowConfig{
		.resizable = true,
		.fullscreen = false,
		.decorated = true
	});

	std::future<gltf::data> gltf_data_future = std::async(loadModelAsync, startup_scene);
	auto const scene_tree = std::make_shared<SceneTree>(window_);
	window_->setSceneTree(scene_tree);
	window_->createSurface(
		true,
		std::nullopt, 
		gfx::ColorSpace::eSrgbNonLinear,
		gfx::PresentMethod::eFifo,
		std::nullopt
	);

	switch (hash("ForwardMulti")) {
		case hash("Forward"):
			window_->setRenderer(std::make_shared<ForwardRenderer>(window_));
			break;
		default:
			printf("Unknown renderer \"%s\" specified in config.ini. Defaulting to ForwardRenderer.\n", renderer_name.c_str());
			window_->setRenderer(std::make_shared<ForwardRenderer>(window_));
			break;
	}

	window_->renderer()->resize(window_->size());

	window_->addSizeChangedCallback([](IWindow *window, ivec2 size) {
		window->renderer()->resize(size);
	});
	
	gltf::data scene_data = gltf_data_future.get();
	uid const root_entity_uid = gltf::createEntityFromGltf(scene_tree, scene_data);
	scene_tree->setRoot(root_entity_uid);

	const SharedPtr<Entity> root_entity = scene_tree->entity(root_entity_uid);
	Result<uid> result_camera_uid = scene_tree->createEntity();
	if (result_camera_uid.error() != OK) _UNLIKELY
		return result_camera_uid.error();

	const SharedPtr<Entity> camera_entity = scene_tree->entity(result_camera_uid.value());
	camera_entity->name_ = "EditorCamera";
	root_entity->addChild(camera_entity);

	editor_camera_ = &camera_entity->component<EditorCamera3D>();
	editor_camera_->setFieldOfVision(glm::radians(60.0f));
	editor_camera_->setAspectRatio((f32)window_->size().x / (f32)window_->size().y);
	editor_camera_->setNearPlane(0.1f);
	editor_camera_->setFarPlane(1000.0f);
	editor_camera_->makeCurrent();
	
	window_->setVisible(true);

	return OK;
}

Result<> DefMainLoop::iter([[maybe_unused]] f64 delta) {
	SharedPtr<SceneTree> const scene_tree = sceneTree();
	SharedPtr<IRenderer> const renderer = window_->renderer();
	window_->pollEvents();
	scene_tree->initiateFrame(delta);
	renderer->requestNewFrame();

	return OK;
}

Result<> DefMainLoop::stop() {
	window_->sceneTree()->dispose();
	IComponentProvider::dispose_all();
	renderer().value()->dispose();
	window_->dispose();
	GraphicsDriver* driver = GraphicsDriver::singleton();
	driver->stop();
	window_ = nullptr;
	return OK;
}

Result<bool> DefMainLoop::running() {
	return !window_->shouldClose();
}

SharedPtr<Window> DefMainLoop::window() const {
	return window_;
}

SharedPtr<IRenderer> DefMainLoop::renderer() const {
	return window_->renderer();
}

SharedPtr<SceneTree> DefMainLoop::sceneTree() const {
	return window_->sceneTree();
}

Result<IRenderer *> DefMainLoop::renderer() {
	return window_->renderer().get();
}