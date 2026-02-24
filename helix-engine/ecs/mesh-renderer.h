#pragma once
#include "ecs.hpp"


class CMeshResource;

/**
 * @brief Encompasses both static meshes and skinned meshes. Skinned meshes also need a Skeleton component
 */
class MeshRenderer : public Component {
public:
	MeshRenderer(CSharedPtr<CSceneTree> const &p_tree, CSharedPtr<CEntity> const &p_entity) : Component(p_tree, p_entity) {}

	void update(double) override;
	
	CUniquePtr<CMeshResource> mesh;

	#ifdef _DEBUG
	void editor() override;
	#endif
};