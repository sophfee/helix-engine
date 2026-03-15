#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>

#include "util.hpp"

#ifdef _DEBUG
#include "imgui/imgui.h"
#endif

CComponentServer<CTransform> CComponentServer<CTransform>::instance_ = CComponentServer();

CTransform::CTransform(CSharedPtr<CSceneTree> const &p_tree, CSharedPtr<CEntity> const &p_entity): Component(p_tree, p_entity) {}



glm::mat4 CTransform::computeTranslation() const {
	glm::mat4 mTranslate(vec4_zero, vec4_zero, vec4_zero, glm::vec4(-translation, 1.0f));
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
	CSharedPtr<CEntity> const parent = entity.lock()->parent();
	TransformMatrices_t local_space_matrices{
		.translate	= computeTranslation(),
		.rotation	= computeRotation(),
		.scale		= computeScale()
	};
	if (parent->hasComponent<CTransform>()) {
		CTransform const &parent_transform = parent->component<CTransform>();
		auto const &[parent_translation_matrix, parent_rotation_matrix, parent_scale_matrix] = parent_transform.computeTransformMatrices();
		return {
			.translate	= local_space_matrices.translate	*	 parent_translation_matrix,
			.rotation	= local_space_matrices.rotation		*	 parent_rotation_matrix,
			.scale		= local_space_matrices.scale		*	 parent_scale_matrix
		};
	}
	return local_space_matrices;
}

glm::mat4 CTransform::matrix() const {
	auto const v_pos = position();
	auto const v_rot = orientation();
	auto const v_scl = scale;
	return glm::translate(glm::mat4(1.0f), v_pos) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), v_scl);
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