#include "main-loop.hpp"

#include <fstream>

#include "Input.h"
#include "util.hpp"
#include "ecs/ecs_gltf.hpp"
#include "ecs/3d/editor/editor_camera.hpp"
#include "ecs/core/scene_tree.hpp"
#include "gpu/gltf.h"
#include "gpu/graphics.hpp"
#include "gpu/renderers/deferred.hpp"
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
	std::future<gltf::data> gltf_data_future = std::async(loadModelAsync, startup_scene);

	auto const &sec_engine_graphics = config_.sections["Engine/Graphics"];
	auto &sec_engine_graphics_window = config_.sections["Engine/Graphics/Window"];
	
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
	
	window_ = std::make_shared<Window>(window_size, window_name, std::nullopt, WindowConfig{
		.resizable = true,
		.fullscreen = fullscreen,
		.decorated = true
	});

	auto const scene_tree = std::make_shared<SceneTree>(window_);
	gltf::data scene_data = gltf_data_future.get();
	uid const root_entity_uid = gltf::createEntityFromGltf(scene_tree, scene_data);
	scene_tree->setRoot(root_entity_uid);

	SharedPtr<Entity> root_entity = scene_tree->entity(root_entity_uid);
	Result<uid> result_camera_uid = scene_tree->createEntity();
	if (result_camera_uid.error() != OK) _UNLIKELY
		return result_camera_uid.error();
	
	SharedPtr<Entity> camera_entity = scene_tree->entity(result_camera_uid.value());
	camera_entity->name_ = "EditorCamera";
	root_entity->addChild(camera_entity);

	editor_camera_ = &camera_entity->component<EditorCamera3D>();
	editor_camera_->makeCurrent();
	
	window_->setSceneTree(scene_tree);

	switch (hash(renderer_name)) {
		case hash("Deferred"):
			window_->setRenderer(std::make_shared<DeferredRenderer>(window_));
			break;
			/* In the future, support forward renderers and such */
		default:
			printf("Unknown renderer \"%s\" specified in config.ini. Defaulting to DeferredRenderer.\n", renderer_name.c_str());
			window_->setRenderer(std::make_shared<DeferredRenderer>(window_));
			break;
	}

	window_->renderer()->resize(window_->getSize());

	window_->setFramebufferSizeCallback([](GLFWwindow *window, int width, int height) {
		auto const window_ = static_cast<Window *>(glfwGetWindowUserPointer(window));
		if (window_ && window_->window == window) {
			window_->setSize(ivec2(width, height));
			if (window_->renderer())
				window_->renderer()->resize(ivec2(width, height));
		}
	});

	window_->makeContextCurrent();
	window_->show();

	return OK;
}

Result<> DefMainLoop::iter([[maybe_unused]] f64 delta) {
	SharedPtr<SceneTree> const scene_tree = sceneTree();

	Input::process(*window_);
	glfwPollEvents();
	
	SharedPtr<IRenderer> const renderer = window_->renderer();

	Result<> const result = renderer->render();
	if (result.error() != OK) _UNLIKELY
		return result;
	scene_tree->initiateFrame(delta);
	
	return OK;
}

Result<> DefMainLoop::stop() {
	GlfwWindowUserPointerEngineData const *const window_data = static_cast<GlfwWindowUserPointerEngineData *>(glfwGetWindowUserPointer(window_->window));
	delete window_data;
	window_->dispose();
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