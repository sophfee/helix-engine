#pragma once

#include "renderer.hpp"
#include "gpu/compositor.h"
#include "gpu/geometry_buffer.hpp"

class ForwardRenderer : public IRenderer {
public:
	ForwardRenderer(SharedPtr<Window> const &window);
	~ForwardRenderer() override;
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
	Result<> resize(ivec2) override;
	Result<> render() override;
	[[nodiscard]] SharedPtr<SceneTree> sceneTree() const override;

protected:

	

private:
	
	Compositor compositor_;
	SharedPtr<Window> window_;
};
