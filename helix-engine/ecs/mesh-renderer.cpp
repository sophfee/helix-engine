#include "mesh-renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "bone-map.h"
#include "imgui.h"
#include "transform.h"

ComponentProvider<StaticMeshRenderer3D> ComponentProvider<StaticMeshRenderer3D>::instance_ = ComponentProvider();

namespace {
	glm::mat4 SearchForModelMatrix(SharedPtr<Entity> const &entity) {
		if (entity->hasComponent<Transform>()) {
			return entity->component<Transform>().matrix();
		}
		return entity->root() ? glm::mat4(1.0) : SearchForModelMatrix(entity->parent());
	}
}


void StaticMeshRenderer3D::draw(RenderPassInfo const &pass_info) {
	std::shared_ptr<Entity> const owner = entity.lock();
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
	mesh->drawAllSubMeshes(pass_info);
}

#ifdef _DEBUG
void StaticMeshRenderer3D::editor() {
	if (ImGui::TreeNodeEx("[C] StaticMeshRenderer3D")) {
		ImGui::Text("%llu primitives", mesh->primitives_.size());
		ImGui::TreePop();
	}
}
#endif
