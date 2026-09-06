#include "main-loop.hpp"

#include <fstream>

#include "imgui_internal.h"
#include "util.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "ecs/ecs_gltf.hpp"
#include "ecs/3d/editor/editor_camera.hpp"
#include "ecs/core/scene_tree.hpp"
#include "gpu/driver.hpp"
#include "gpu/gltf.h"
#include "gpu/graphics.hpp"
#include "gpu/driver.hpp"
#include "gpu/lighting.hpp"
#include "gpu/window.hpp"
#include "gpu/backends/vulkan_backend.hpp"
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

Result<> Main::iterate(f64 const delta) {
	if (!main_loop_) _UNLIKELY
		return FAILED;
	
	Result<> const result = main_loop_->iterate(delta);
	return result;
}

Result<> Main::stop() {
	return main_loop_->stop();
}

Result<bool> Main::is_running() {
	return main_loop_->is_running();
}

Result<IRenderer *> Main::get_renderer() {
	return main_loop_->get_renderer();
}

Result<IMainLoop &> Main::get_main_loop() {
	if (!main_loop_) _UNLIKELY
		return FAILED;
	return *main_loop_;
}

namespace {
	gltf::Data load_model_async(std::string const &path) {
		auto gltf_path = simdjson::padded_string::load(path).value();
		return gltf::parse(path, std::move(gltf_path));
	}
}

Result<> DefMainLoop::start(std::string const &startup_scene) {
	std::ifstream config_stream("config.ini");
	config_.parse(config_stream);

	String renderer_name;
	const Map<String, String> &sec_engine_graphics = config_.sections["Engine/Graphics"];
	const Map<String, String> &sec_engine_graphics_window = config_.sections["Engine/Graphics/Window"];
	
	WindowDriver windowing = WindowDriver::eSdl2;
	String window_driver;
	inipp::get_value(sec_engine_graphics_window, "WindowingDriver", window_driver);
	
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
	
	inipp::get_value(sec_engine_graphics, "Renderer", renderer_name);

	ivec2 window_size;
	inipp::get_value(sec_engine_graphics_window, "Width", window_size.x);
	inipp::get_value(sec_engine_graphics_window, "Height", window_size.y);

	String window_name;
	inipp::get_value(sec_engine_graphics_window, "Title", window_name);

	bool fullscreen;
	inipp::get_value(sec_engine_graphics_window, "Fullscreen", fullscreen);
	
#ifdef _DEBUG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	//SetupImGuiDraculaStyle();
#endif
	
	GraphicsSystem* driver = GraphicsSystem::get_singleton();
	driver->set_backend(RenderingApiBackend::eVulkan);
	
	window_ = std::make_shared<Window>(windowing, RenderingApiBackend::eVulkan, window_size, window_name, std::nullopt, WindowConfig{
		.resizable = true,
		.fullscreen = false,
		.decorated = true
	});
	std::future<gltf::Data> gltf_data_future = std::async(load_model_async, startup_scene);
	auto const scene_tree = std::make_shared<SceneTree>(window_);
	window_->set_scene_tree(scene_tree);
	window_->create_surface(
		true,
		std::nullopt, 
		gfx::ColorSpace::eSrgbNonLinear,
		gfx::PresentMethod::eMailbox,
		std::nullopt
	);
	
	dynamic_cast<VkGraphicsDriverBackend*>(GraphicsSystem::get_driver())->initialize_im_gui();

	switch (hash("ForwardMulti")) {
		case hash("Forward"):
			window_->set_renderer(std::make_shared<ForwardRenderer>(window_));
			break;
		default:
			printf("Unknown renderer \"%s\" specified in config.ini. Defaulting to ForwardRenderer.\n", renderer_name.c_str());
			window_->set_renderer(std::make_shared<ForwardRenderer>(window_));
			break;
	}

	window_->get_renderer()->resize(window_->get_size());

	window_->add_size_changed_callback([](IWindow *window, ivec2 size) {
		window->get_renderer()->resize(size);
	});
	LightingSystem *lighting_system = LightingSystem::singleton();
	
	gltf::Data scene_data = gltf_data_future.get();
	RID const root_entity_uid = gltf::create_entity_from_gltf(scene_tree, scene_data);
	scene_tree->set_root(root_entity_uid);

	Entity* root_entity = scene_tree->get_entity(root_entity_uid);
	root_entity->scene_tree_ = scene_tree;
	
	Result<RID>result_camera_uid = scene_tree->create_entity();
	if (result_camera_uid.error() != OK) _UNLIKELY
		return result_camera_uid.error();

	Entity* camera_entity = scene_tree->get_entity(result_camera_uid.value());
	camera_entity->name_ = "EditorCamera";
	camera_entity->scene_tree_ = scene_tree;
	
	root_entity = scene_tree->get_entity(root_entity_uid);
	root_entity->add_child(camera_entity);

	auto& cam = camera_entity->get_component<EditorCamera3D>();
	cam.set_field_of_vision(glm::radians(89.0f));
	cam.set_aspect_ratio((f32)window_->get_size().x / (f32)window_->get_size().y);
	cam.set_near_plane(0.05f);
	cam.set_far_plane(1000.0f);
	cam.make_current();
	
	editor_camera_ = std::addressof(cam);
	window_->set_visible(true);

	return OK;
}

Result<> DefMainLoop::iterate([[maybe_unused]] f64 delta) {
	SharedPtr<SceneTree> const scene_tree = get_scene_tree();
	SharedPtr<IRenderer> const renderer = window_->get_renderer();
	window_->poll_events();
	
	scene_tree->init_frame(delta);
	scene_tree->draw_editors();
	renderer->request_new_frame();
	
	return OK;
}

Result<> DefMainLoop::stop() {
	GraphicsSystem* driver = GraphicsSystem::get_singleton();
	GraphicsSystem::get_driver()->yield_for_commands();
	window_->get_scene_tree()->dispose();
	IComponentProvider::dispose_all();
	get_renderer().value()->dispose();
	window_->dispose();
	driver->shutdown();
	window_ = nullptr;
	return OK;
}

Result<bool> DefMainLoop::is_running() {
	return !window_->get_should_close();
}

SharedPtr<Window> DefMainLoop::get_window() const {
	return window_;
}

SharedPtr<IRenderer> DefMainLoop::get_renderer() const {
	return window_->get_renderer();
}

SharedPtr<SceneTree> DefMainLoop::get_scene_tree() const {
	return window_->get_scene_tree();
}

Result<IRenderer *> DefMainLoop::get_renderer() {
	return window_->get_renderer().get();
}