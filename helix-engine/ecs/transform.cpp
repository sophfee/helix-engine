#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>

#include "util.hpp"

#ifdef _DEBUG
#include "imgui/imgui.h"
#endif

ComponentServer<Transform> ComponentServer<Transform>::instance_ = ComponentServer();

Transform::Transform(SharedPtr<SceneTree> const &p_tree, SharedPtr<Entity> const &p_entity): Component(p_tree, p_entity) {}



mat4 Transform::computeTranslation() const {
	mat4 mTranslate = glm::translate(mat4(1.0), translation);
	return mTranslate;
}

vec3 Transform::position() const {
	SharedPtr<Entity> const parent = entity.lock()->parent();
	vec3 pos = translation;
	if (parent->hasComponent<Transform>()) {
		Transform const &parent_transform = parent->component<Transform>();
		pos += parent_transform.position();
	}
	return pos;
}

mat4 Transform::computeRotation() const {
	return glm::mat4_cast(rotation);
}

quat Transform::orientation() const {
	SharedPtr<Entity> const parent = entity.lock()->parent();
	quat rot = rotation;
	if (parent->hasComponent<Transform>()) {
		Transform const &parent_transform = parent->component<Transform>();
		rot = rot * parent_transform.orientation();
	}
	return rot;
}
mat4 Transform::computeScale() const {
	return glm::scale(mat4(1.0f), scale);
}

TransformMatrices_t Transform::computeTransformMatrices() const {
	return {
		.translate	= computeTranslation(),
		.rotation	= computeRotation(),
		.scale		= computeScale()
	};
}

mat4 Transform::matrix() const {
#if 1
	SharedPtr<Entity> const parent = entity.lock()->parent();
#ifdef TRANSFORM_OTHER_METHOD
	mat4 myTransform(
		scale.x,     0.f,     0.f, 0.f,
		    0.f, scale.y,     0.f, 0.f,
		    0.f,     0.f, scale.z, 0.f,
		    0.f,     0.f,     0.f, 1.f
	);
	myTransform = glm::mat4_cast(rotation) * myTransform;
	myTransform[3] = vec4(translation.x, translation.y, translation.z, 1.f);
#else
	auto [t, r, s] = computeTransformMatrices();
	mat4 myTransform;
	switch (order) {
		case TranslateRotateScale: myTransform = t * r * s; break;
		case ScaleTranslateRotate: myTransform = s * t * r; break;
		case RotateScaleTranslate: myTransform = r * s * t; break;
		case RotateTranslateScale: myTransform = r * t * s; break;
		case ScaleRotateTranslate: myTransform = s * r * t; break;
		case TranslateScaleRotate: myTransform = t * s * r; break;
	}
#endif
	if (parent->hasComponent<Transform>()) {
		Transform const &parent_transform = parent->component<Transform>();
		myTransform = parent_transform.matrix() * myTransform;
	}
	return myTransform;
#else
	
	auto const v_pos = translation;
	auto const v_rot = rotation;
	auto const v_scl = scale;
	auto const v_mtx = glm::translate(glm::mat4(1.0f), v_pos) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), v_scl);
	SharedPtr<Entity> const parent = entity.lock()->parent();
	if (parent->hasComponent<Transform>()) {
		Transform const &parent_transform = parent->component<Transform>();
		return parent_transform.matrix() * v_mtx;
	}
	return v_mtx;
#endif
}

#ifdef _DEBUG
void Transform::editor() {
	if (ImGui::TreeNode("Transform")) {
		ImGui::InputFloat3("Translation", &translation[0]);
		ImGui::InputFloat4("Quaternion", &rotation[0]);
		ImGui::InputFloat3("Scale", &scale[0]);
		ImGui::TreePop();
	}
}
#endif