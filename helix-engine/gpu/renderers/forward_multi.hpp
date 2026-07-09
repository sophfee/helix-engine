#pragma once

#include "renderer.hpp"
#include "gpu/compositor.h"
#include "gpu/geometry_buffer.hpp"

class ForwardMultiDrawRenderer : public IRenderer {
public:
	ForwardMultiDrawRenderer(SharedPtr<Window> const &window);
	~ForwardMultiDrawRenderer() override;
	
	Result<> resize(ivec2) override;
	Result<> render() override;
	[[nodiscard]] SharedPtr<SceneTree> sceneTree() const override;
	RendererType rendererType() const override;

	void dispose() override;
	[[nodiscard]] bool disposed() const override;

protected:

	TypedBuffer<FrameData> frame_data_buffer_;
	FrameData *frame_data_ = nullptr;
	Program mesh_cull_;
	Program forward_draw_;
	
private:
	
	Compositor compositor_;
	SharedPtr<Window> window_;
};
