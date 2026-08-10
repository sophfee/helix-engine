#pragma once

#include "types.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include "ecs/core/core_includes.hpp"
#include "engine/rid.hpp"

class EditorCamera3D;

class ForwardRenderer : public IRenderer {
public:
	ForwardRenderer(SharedPtr<Window> const &window);
	
	Result<> resize(ivec2) override;
	Result<> render() override;

	[[nodiscard]] SharedPtr<SceneTree> sceneTree() const override;
	RendererType rendererType() const override;
	
	[[nodiscard]] RID primaryBindGroupLayout() const override;
	void requestNewFrame() override;
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

public:
	std::binary_semaphore render_semaphore{1};
	std::future<void> render_future;
	
	uid camera;
	EditorCamera3D* editor_camera_;
	RID shader;
	RID pipeline;
	RID pipeline_layout;
	RID bind_group_layout;
	
	bool is_disposed_ = false;
	
private:
	SharedPtr<Window> window_;
};
