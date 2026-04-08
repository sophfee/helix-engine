#pragma once

#include "ecs.hpp"
#include "core/component.hpp"

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

enum EMatrixOperationOrder : u8 {
	TranslateRotateScale,
	TranslateScaleRotate,
	ScaleTranslateRotate,
	ScaleRotateTranslate,
	RotateTranslateScale,
	RotateScaleTranslate
};

class Transform : public Component {
public:
	Transform(SharedPtr<SceneTree> const &p_tree, SharedPtr<Entity> const &p_entity);

	EMatrixOperationOrder order = TranslateRotateScale;
	vec3 translation = vec3(0.0f);
	vec3 scale = vec3(1.0f);
	quat rotation = quat();

	// Only computes local space translation!
	_NODISCARD mat4 computeTranslation() const;

	_NODISCARD vec3 position() const;
	
	// Only computes local space rotation!
	_NODISCARD mat4 computeRotation() const;

	_NODISCARD quat orientation() const;

	_NODISCARD vec3 right() const;
	_NODISCARD vec3 left() const;
	_NODISCARD vec3 up() const;
	_NODISCARD vec3 down() const;
	_NODISCARD vec3 forward() const;
	_NODISCARD vec3 backward() const;
	
	// Only computes local space scale!
	_NODISCARD mat4 computeScale() const;
	// Performs hierarchy calculated matrix transformation
	_NODISCARD TransformMatrices_t computeTransformMatrices() const;
	
	[[nodiscard]] mat4 matrix() const;

#ifdef _DEBUG
	void editor() override;
#endif
};
