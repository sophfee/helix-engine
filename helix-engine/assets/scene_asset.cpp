#include "scene_asset.hpp"

#include "ecs/core/entity.hpp"
#include "ecs/core/scene_tree.hpp"

void SceneAsset::instantiate(const SharedPtr<SceneTree> &scene_tree, EntityRef& entity_ref) const {
	scene_tree->create_entity_to_ref(entity_ref);
}

StringView SceneAsset::get_class_name() const {
	return "SceneAsset";
}

void SceneAsset::set(const std::string_view property, const Any &value) {
	Asset::set(property, value);
}

Any SceneAsset::get(const std::string_view property) const {
	return Asset::get(property);
}
