#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>

#include "util.hpp"

#ifdef _DEBUG
#include "imgui/imgui.h"
#endif

ComponentProvider<Transform> ComponentProvider<Transform>::instance_ = ComponentProvider();

Transform::Transform(): Component() {
}

Transform::Transform(SharedPtr<SceneTree> const &p_tree, const RID p_entity): Component(p_tree, p_entity) {}



mat4 Transform::compute_translation() const {
	mat4 mTranslate = glm::translate(mat4(1.0), translation);
	return mTranslate;
}

vec3 Transform::get_position() const {
	const Entity* parent = get_entity()->get_parent();
	vec3 pos = translation;
	//if (parent->hasComponent<Transform>()) {
	//	Transform const &parent_transform = parent->component<Transform>();
	//	pos += parent_transform.position();
	//}
	return pos;
}

mat4 Transform::compute_rotation() const {
	return glm::mat4_cast(rotation);
}

quat Transform::get_orientation() const {
	const Entity* parent = get_entity()->get_parent();
	quat rot = rotation;
	if (parent->has_component<Transform>()) {
		Transform const &parent_transform = parent->get_component<Transform>();
		rot = rot * parent_transform.get_orientation();
	}
	return rot;
}

vec3 Transform::get_right() const {
	return vec3(get_matrix()[0]);
}

vec3 Transform::get_left() const {
	return -vec3(get_matrix()[0]);
}

vec3 Transform::get_up() const {
	return vec3(get_matrix()[1]);
}

vec3 Transform::get_down() const {
	return -vec3(get_matrix()[1]);
}

vec3 Transform::get_backward() const {
	return vec3(get_matrix()[2]);
}

vec3 Transform::get_forward() const {
	return -vec3(get_matrix()[2]);
}

mat4 Transform::compute_scale() const {
	return glm::scale(mat4(1.0f), scale);
}

TransformMatrices_t Transform::compute_transform_matrices() const {
	return {
		.translate	= compute_translation(),
		.rotation	= compute_rotation(),
		.scale		= compute_scale()
	};
}

mat4 Transform::get_matrix() const {
#if 1
	// const Entity* parent = entity()->parent();
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
	auto [t, r, s] = compute_transform_matrices();
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
	//if (parent->hasComponent<Transform>()) {
	//	//Transform const &parent_transform = parent->component<Transform>();
	//	//myTransform = parent_transform.matrix() * myTransform;
	//}
	return myTransform;
#else
	
	auto const v_pos = translation;
	auto const v_rot = rotation;
	auto const v_scl = scale;
	auto const v_mtx = glm::translate(glm::mat4(1.0f), v_pos) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), v_scl);
	const Entity* parent = get_entity()->get_parent();
	if (parent->has_component<Transform>()) {
		Transform const &parent_transform = parent->get_component<Transform>();
		return parent_transform.get_matrix() * v_mtx;
	}
	return v_mtx;
#endif
}

#ifdef _DEBUG
void Transform::editor() {
	ImGui::Spacing();
	ImGui::SeparatorText("Component: Transform");
	
	if (ImGui::InputFloat3("Position", &translation[0])) {
		dirty_[0] = true;
		dirty_[1] = true;
	}
	
	if (ImGui::InputFloat4("Quaternion", &rotation[0])) {
		dirty_[0] = true;
		dirty_[1] = true;
	}
	
	if (ImGui::InputFloat3("Scale", &scale[0])) {
		dirty_[0] = true;
		dirty_[1] = true;
	}
}
#endif
