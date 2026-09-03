#pragma once

#include "core/core_includes.hpp"
#include "core/scene_tree.hpp"

namespace gltf {
	struct Data;
	
	/**
	 * @see gltf::parse
	 * @param scene_tree
	 * @param data The gltf data that has been parsed already
	 * @return The root entity id, note that it has yet to be added to the scene hierarchy as it is not the child of any entity.
	 */
	extern RID create_entity_from_gltf(SharedPtr<SceneTree> const &scene_tree, Data &data);
}