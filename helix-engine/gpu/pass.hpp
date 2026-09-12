#pragma once

#include "types.hpp"
#include "ecs/core/scene_tree.hpp"

class Component;
class IRenderer;

/**
 * \brief Interface for the lowest level GPU pass abstraction. This is used to record commands into a command buffer
 */
class IGpuPass {
public:
	virtual void record(IRenderer* renderer, RID command, Optional<RID> surface) = 0;
};

/**
 * \brief Like <code>IGpuPass</code> but is called once for each Component of type <code>T</code> in the scene.
 * This is used to record commands into a command buffer for each component of type <code>T</code>.
 * Keep in mind, as of now, ECS doesn't work with polymorphism, you will only get
 * active Components of the exact time specified.
 */
template <typename T> requires std::is_base_of_v<Component, T>
class IGpuPassComponentVisitor {
public:
	virtual void record(IRenderer* renderer, RID command, Optional<RID> surface, T* component) = 0;

	template <typename X>
	void visit(SceneTree* scene_tree, IRenderer* renderer, RID command, Optional<RID> surface) {
		scene_tree->visit_component([](T* component, std::decay_t<X>* self, IRenderer* renderer, RID command, Optional<RID> surface) {
			self->record(renderer, command, surface, component);
		}, 0, dynamic_cast<X*>(this), renderer, command, surface);
	}
};