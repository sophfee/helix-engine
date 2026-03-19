#include "mesh-renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "bone-map.h"
#include "imgui.h"
#include "transform.h"

CComponentServer<CMeshRenderer> CComponentServer<CMeshRenderer>::instance_ = CComponentServer();

namespace {
	glm::mat4 SearchForModelMatrix(CSharedPtr<CEntity> const &entity) {
		if (entity->hasComponent<CTransform>()) {
			return entity->component<CTransform>().matrix();
		}
		return entity->root() ? glm::mat4(1.0) : SearchForModelMatrix(entity->parent());
	}
}

void CMeshRenderer::update(double x) {
	std::shared_ptr<CEntity> const owner = entity.lock();
	glm::mat4 model = SearchForModelMatrix(owner);
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(model));
	gpu_check;
	if (owner->hasComponent<BoneMap>()) {
		BoneMap const &bone_map = owner->component<BoneMap>();
		//bone_map.updateBuffer();
		bone_map.bindBuffer();
	}
	glUniform1i(10, owner->debug_hovered_ ? 1 : 0);
	gpu_check;
	mesh->drawAllSubMeshes();
}

#ifdef _DEBUG
void CMeshRenderer::editor() {
	if (ImGui::TreeNodeEx("[C] CMeshRenderer")) {
		ImGui::Text("%llu primitives", mesh->primitives_.size());
		ImGui::TreePop();
	}
}
#endif
