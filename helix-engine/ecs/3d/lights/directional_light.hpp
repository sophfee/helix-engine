#pragma once

#include "ecs/ecs.hpp"

class Environment;

class DirectionalLight : public Component {
public:
	DirectionalLight(Weak<SceneTree> const &scene_tree, Weak<Entity> const &ent);

	friend class Environment;
};