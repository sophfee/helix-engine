#pragma once

#include "ecs.hpp"

struct TransformMatrices_t {
	mat4 translate;
	mat4 rotation;
	mat4 scale;
};

struct transform_data {
	vec3 position;
	quat rotation;
	vec3 scale;
};

class Transform : public Component {
public:
	Transform(SharedPtr<SceneTree> const &p_tree, SharedPtr<Entity> const &p_entity);

	vec3 translation = vec3(0.0f);
	vec3 scale = vec3(1.0f);
	quat rotation = quat();

	// Only computes local space translation!
	_NODISCARD mat4 computeTranslation() const;

	_NODISCARD vec3 position() const;
	
	// Only computes local space rotation!
	_NODISCARD mat4 computeRotation() const;

	_NODISCARD quat orientation() const;
	
	// Only computes local space scale!
	_NODISCARD mat4 computeScale() const;
	// Performs hierarchy calculated matrix transformation
	_NODISCARD TransformMatrices_t computeTransformMatrices() const;
	
	[[nodiscard]] mat4 matrix() const;

#ifdef _DEBUG
	void editor() override;
#endif
};
