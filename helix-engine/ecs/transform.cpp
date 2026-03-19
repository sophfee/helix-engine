#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>

#include "util.hpp"

#ifdef _DEBUG
#include "imgui/imgui.h"
#endif

CComponentServer<CTransform> CComponentServer<CTransform>::instance_ = CComponentServer();

CTransform::CTransform(CSharedPtr<CSceneTree> const &p_tree, CSharedPtr<CEntity> const &p_entity): Component(p_tree, p_entity) {}



glm::mat4 CTransform::computeTranslation() const {
	glm::mat4 mTranslate = glm::translate(glm::mat4(1.0), translation);
	return mTranslate;
}

glm::vec3 CTransform::position() const {
	CSharedPtr<CEntity> const parent = entity.lock()->parent();
	glm::vec3 pos = translation;
	if (parent->hasComponent<CTransform>()) {
		CTransform const &parent_transform = parent->component<CTransform>();
		pos += parent_transform.position();
	}
	return pos;
}

glm::mat4 CTransform::computeRotation() const {
	return glm::mat4_cast(rotation);
}
glm::quat CTransform::orientation() const {
	CSharedPtr<CEntity> const parent = entity.lock()->parent();
	glm::quat rot = rotation;
	if (parent->hasComponent<CTransform>()) {
		CTransform const &parent_transform = parent->component<CTransform>();
		rot = rot * parent_transform.orientation();
	}
	return rot;
}
glm::mat4 CTransform::computeScale() const {
	return glm::scale(glm::mat4(1.0f), scale);
}

TransformMatrices_t CTransform::computeTransformMatrices() const {
	return {
		.translate	= computeTranslation(),
		.rotation	= computeRotation(),
		.scale		= computeScale()
	};
}

glm::mat4 CTransform::matrix() const {
#if 1
	CSharedPtr<CEntity> const parent = entity.lock()->parent();
	auto [t, r, s] = computeTransformMatrices();
	glm::mat4 myTransform(
		scale.x,     0.f,     0.f, 0.f,
		    0.f, scale.y,     0.f, 0.f,
		    0.f,     0.f, scale.z, 0.f,
		    0.f,     0.f,     0.f, 1.f
	);
	myTransform = glm::mat4_cast(rotation) * myTransform;
	myTransform[3] = glm::vec4(translation.x, translation.y, translation.z, 1.f);
	if (parent->hasComponent<CTransform>()) {
		CTransform const &parent_transform = parent->component<CTransform>();
		myTransform = parent_transform.matrix() * myTransform;
	}
	return myTransform;
#else
	
	auto const v_pos = translation;
	auto const v_rot = rotation;
	auto const v_scl = scale;
	auto const v_mtx = glm::translate(glm::mat4(1.0f), v_pos) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), v_scl);
	CSharedPtr<CEntity> const parent = entity.lock()->parent();
	if (parent->hasComponent<CTransform>()) {
		CTransform const &parent_transform = parent->component<CTransform>();
		return parent_transform.matrix() * v_mtx;
	}
	return v_mtx;
#endif
}

#ifdef _DEBUG
void CTransform::editor() {
	if (ImGui::TreeNode("Transform")) {
		ImGui::InputFloat3("Translation", &translation[0]);
		ImGui::InputFloat4("Quaternion", &rotation[0]);
		ImGui::InputFloat3("Scale", &scale[0]);
		ImGui::TreePop();
	}
}
#endif