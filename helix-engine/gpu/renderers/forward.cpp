#include "forward.hpp"

ForwardRenderer::ForwardRenderer(SharedPtr<Window> const &window) : IRenderer(window),
	forward_draw_("shaders\\forward_simple.vert", "shaders\\forward_simple.frag"){}

Result<> ForwardRenderer::resize(ivec2) {
	return OK;
}

Result<> ForwardRenderer::render() {
	
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
