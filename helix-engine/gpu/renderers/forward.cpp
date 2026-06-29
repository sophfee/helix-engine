#include "forward.hpp"

ForwardRenderer::ForwardRenderer(SharedPtr<Window> const &window) : IRenderer(window), compositor_(window->getSize()), window_(window) {
	
}

ForwardRenderer::~ForwardRenderer() {
}

void ForwardRenderer::dispose() {
	compositor_.dispose();
}

bool ForwardRenderer::disposed() const {
	return compositor_.disposed();
}

Result<> ForwardRenderer::resize(ivec2 const desired_resolution) {
	compositor_.resize(desired_resolution);
	return OK;
}
Result<> ForwardRenderer::render() {
	return OK;
}

SharedPtr<SceneTree> ForwardRenderer::sceneTree() const {
	return window_->sceneTree();
}
