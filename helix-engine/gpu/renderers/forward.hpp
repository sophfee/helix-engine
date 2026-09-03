#pragma once

#include "types.hpp"
#include "math.hpp"
#include "renderer.hpp"
#include "ecs/core/core_includes.hpp"
#include "engine/rid.hpp"
#include "gpu/geometry.hpp"
#include "gpu/mesh.hpp"

struct Vertex;
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

	RendererType get_renderer_type() const override;
	
	[[nodiscard]] RID get_primary_bind_group_layout() const override;
	void request_new_frame() override;
	
	[[nodiscard]] const SceneData &get_scene_data() const override;
	[[nodiscard]] SceneData &get_scene_data_mutable() override;
	[[nodiscard]] RID get_scene_data_rid() const override;
	
	[[nodiscard]] SharedPtr<SceneTree> get_scene_tree() const override;
	
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
	
	RID camera;
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