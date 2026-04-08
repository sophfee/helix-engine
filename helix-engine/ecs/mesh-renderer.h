#pragma once

#include "core/component.hpp"

class Mesh;

/**
 * @brief Encompasses both static meshes and skinned meshes. Skinned meshes also need a Skeleton component
 */
class StaticMeshRenderer3D : public Component {
	bool m_bDbgHovering = false;
public:
	StaticMeshRenderer3D(SharedPtr<SceneTree> const &p_tree, SharedPtr<Entity> const &p_entity) : Component(p_tree, p_entity) {}

	bool culled(RenderPassInfo const &pass_info);
	void draw(RenderPassInfo const &pass_info) override;
	
	
	UniquePtr<Mesh> mesh;
	bool wasMostRecentlyCulled = false;
	i32 primitives_drawn_ = 0;

	#ifdef _DEBUG
	void editor() override;
	#endif
};