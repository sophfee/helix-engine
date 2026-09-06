#pragma once

#include "core/component.hpp"
#include "engine/rid.hpp"

struct PerModelData;
struct GpuMeshTransform;
class Mesh;

/**
 * @brief Encompasses both static meshes and skinned meshes. Skinned meshes also need a Skeleton component
 */
class StaticMeshRenderer3D : public Component {
	bool open_inspector = false;
	bool hovering_ = false;
	bool wasMostRecentlyCulled = false;
	bool just_cleaned = false;
	RID bind_group_layout = 0;
	i32 primitives_drawn_ = 0;
public:
	StaticMeshRenderer3D();
	StaticMeshRenderer3D(SharedPtr<SceneTree> const &p_tree, const RID p_entity);

	bool culled(RenderPassInfo const &pass_info);
	void update(double) override;
	void draw(RenderPassInfo const &pass_info) override;
	void render_setup(RenderPassInfo const &pass_info) override;
	void destroy() override;
	
	UniquePtr<Mesh> mesh;
	
	PerModelData* transform_;
	RID transform_buffer_;
	RID desc_set;

	#ifdef _DEBUG
	void editor() override;
#endif

};