#pragma once

#include "types.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include "ecs/core/core_includes.hpp"
#include "engine/rid.hpp"
#include "gpu/geometry.hpp"

class EditorCamera3D;
struct CulledData
{
	uint32_t frustumCulled{0};
	uint32_t backfaceCulled{0};
	uint32_t totalCulled{0};
};
class ForwardRenderer : public IRenderer {
public:
	ForwardRenderer(SharedPtr<Window> const &window);
	
	Result<> resize(ivec2) override;
	Result<> render() override;

	[[nodiscard]] SharedPtr<SceneTree> sceneTree() const override;
	RendererType rendererType() const override;
	
	[[nodiscard]] RID primaryBindGroupLayout() const override;
	void requestNewFrame() override;
	
	[[nodiscard]] const SceneData &sceneData() const override;
	[[nodiscard]] SceneData &sceneDataMut() override;
	[[nodiscard]] RID sceneDataRid() const override;
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

public:
	SceneData scene_data_;
	SceneData* scene_data_mapped_address_;
	CulledData culled_data_;
	CulledData* culled_data_mapped_address_;
	RID scene_data_rid_;
	RID culled_data_rid_;
	
	std::binary_semaphore render_semaphore{1};
	std::future<void> render_future;
	
	uid camera;
	EditorCamera3D* editor_camera_;
	RID shader;
	RID pipeline;
	RID pipeline_layout;
	RID bind_group_layout;
	f64 timer_ = 0.0;
	
	bool is_disposed_ = false;
	
private:
	SharedPtr<Window> window_;
};