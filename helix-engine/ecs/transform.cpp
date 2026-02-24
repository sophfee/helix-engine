#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>

#ifdef _DEBUG
#include "imgui/imgui.h"
#endif

CComponentServer<Transform> CComponentServer<Transform>::instance_ = CComponentServer();

Transform::Transform(CSharedPtr<CSceneTree> const &p_tree, CSharedPtr<CEntity> const &p_entity): Component(p_tree, p_entity) {}


glm::mat4 Transform::matrix() const {
	CSharedPtr<CEntity> const parent = entity.lock()->parent();
	glm::mat4 mat = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation);
	mat = glm::scale(mat, scale);
	if (parent->hasComponent<Transform>()) {
		Transform const &parentTransform = parent->component<Transform>();
		mat = parentTransform.matrix() * mat; // recursively apply all matrices.
	}
	return mat;
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