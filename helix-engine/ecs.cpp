#include "ecs.hpp"

#ifdef COMPILE_ECS

// CEntity

CEntity::~CEntity() {
	const Error err = scene_tree_->removeEntity(this->unique_id_);
	assert(err == OK);
}

std::shared_ptr<IComponentInstance> CEntity::getComponentInstance(uid id) const {
	return nullptr;
}

//
// CSceneTree
//

CSceneTree::CSceneTree() {}
CSceneTree::~CSceneTree() {}
void CSceneTree::initiateFrame() {}

#endif