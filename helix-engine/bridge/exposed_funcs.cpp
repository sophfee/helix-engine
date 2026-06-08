#include "exposed_funcs.hpp"

#include "ecs/core/scene_tree.hpp"
#include "glad/glad.h"

int32_t engine_get_active_window(void **pWindow) {return 0;}
int32_t engine_get_scene_tree(void **pSceneTree) {
	return 0;
}

int32_t scenetree_get_entity(void *pSceneTree, uint32_t entity_id, void **pEntity) {
	SceneTree *sceneTree = (SceneTree *)pSceneTree;
	SharedPtr<Entity> const entity = sceneTree->entity(entity_id);
	*pEntity = entity.get();
	return OK;
}
int32_t scenetree_root(void *pSceneTree, void **pRootEntity) {
	SceneTree *sceneTree = (SceneTree *)pSceneTree;
	return 0;
}

int32_t scenetree_get_window(void *pSceneTree, void **pWindow) {
	return 0;
}

int32_t scenetree_visit_entities(void *pSceneTree, scenetree_entity_visitor_fn visitor, void **user_data) {
	return 0;
}

int32_t scenetree_visit_component(void *pSceneTree, uint32_t component_id, void *user_data) {
	return 0;
}

#pragma region texture

int32_t texture_create(void **pTexture, texture_create_info_t const *pCreateInfo) {
	return 0;
}

int32_t texture_free(void *pTexture) {
	return 0;
}

#pragma endregion 