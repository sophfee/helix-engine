#include "mesh-renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "bone-map.h"
#include "imgui.h"
#include "transform.h"

CComponentServer<MeshRenderer> CComponentServer<MeshRenderer>::instance_ = CComponentServer();

namespace {
	glm::mat4 SearchForModelMatrix(CSharedPtr<CEntity> const &entity) {
		if (entity->hasComponent<Transform>()) {
			return entity->component<Transform>().matrix();
		}
		return entity->root() ? glm::mat4(1.0) : SearchForModelMatrix(entity->parent());
	}
}

void MeshRenderer::update(double x) {
	auto const owner = entity.lock();
	glm::mat4 model = SearchForModelMatrix(owner);
	glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, glm::value_ptr(model));
	gpu_check;
	if (owner->hasComponent<BoneMap>()) {
		BoneMap const &bone_map = owner->component<BoneMap>();
		//bone_map.updateBuffer();
		bone_map.bindBuffer();
	}
	gpu_check;
	mesh->drawAllSubMeshes();
}

#ifdef _DEBUG
void MeshRenderer::editor() {
	if (ImGui::TreeNodeEx("[C] MeshRenderer")) {
		ImGui::Text("%llu primitives", mesh->primitives_.size());
		ImGui::TreePop();
	}
}
#endif
