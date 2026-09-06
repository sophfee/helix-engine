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
	Transform();
	Transform(SharedPtr<SceneTree> const &p_tree, const RID p_entity);

	EMatrixOperationOrder order = TranslateRotateScale;
	vec3 translation = vec3(0.0f);
	vec3 scale = vec3(1.0f);
	quat rotation = quat();
	mutable bool dirty_[2] = {true, true};

	// Only computes local space translation!
	_NODISCARD mat4 compute_translation() const;

	_NODISCARD vec3 get_position() const;
	
	// Only computes local space rotation!
	_NODISCARD mat4 compute_rotation() const;

	_NODISCARD quat get_orientation() const;

	_NODISCARD vec3 get_right() const;
	_NODISCARD vec3 get_left() const;
	_NODISCARD vec3 get_up() const;
	_NODISCARD vec3 get_down() const;
	_NODISCARD vec3 get_forward() const;
	_NODISCARD vec3 get_backward() const;
	
	// Only computes local space scale!
	_NODISCARD mat4 compute_scale() const;
	// Performs hierarchy calculated matrix transformation
	_NODISCARD TransformMatrices_t compute_transform_matrices() const;
	
	[[nodiscard]] mat4 get_matrix() const;

#ifdef _DEBUG
	void editor() override;
#endif
};
