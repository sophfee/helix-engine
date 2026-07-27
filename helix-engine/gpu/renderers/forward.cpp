#include "forward.hpp"

#include "ecs/core/scene_tree.hpp"
#include "gpu/driver.hpp"
#include "gpu/graphics.hpp"
#include "gpu/window.hpp"

ForwardRenderer::ForwardRenderer(SharedPtr<Window> const &window) : IRenderer(window), window_(window) {
	GraphicsDriver* driver = GraphicsDriver::singleton();
	
	shader = driver->shader_create();
	driver->shader_load_spirv_from_file(shader, "shaders/vulkan/standard.spv");
	
	pipeline = driver->pipeline_create(window.get(), shader);
	
	camera = window_->sceneTree()->createEntity();
	editor_camera_ = &window_->sceneTree()->entity(camera)->component<EditorCamera3D>();
}

Result<> ForwardRenderer::resize(ivec2) {
	return OK;
}

Result<> ForwardRenderer::render() {
	GraphicsDriver* driver = GraphicsDriver::singleton();
	const RID cmd = driver->start_recording(window_.get());
	driver->start_rendering(window_.get(), cmd, pipeline);
	
	sceneTree()->initiateRenderSetup({
		.pass = RenderPassType::Normal,
		.pipeline = pipeline,
		.cmd = cmd
	});
	
	sceneTree()->initiateDraw({
		.pass = RenderPassType::Normal,
		.pipeline = pipeline,
		.cmd = cmd
	});
	
	driver->stop_rendering(window_.get(), cmd);
	
	driver->stop_recording(window_.get(), cmd);
	
	driver->submit(window_.get(), cmd);
	driver->present(window_.get());
	
	return OK;
}
SharedPtr<SceneTree> ForwardRenderer::sceneTree() const {
	return window_->sceneTree();
}

RendererType ForwardRenderer::rendererType() const {
	return RendererType::FORWARD;
}

void ForwardRenderer::dispose() {
	is_disposed_ = true;
}

bool ForwardRenderer::disposed() const {
	return is_disposed_;
}
