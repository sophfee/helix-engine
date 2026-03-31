#include "mesh-renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "bone-map.h"
#include "imgui.h"
#include "transform.h"

ComponentProvider<StaticMeshRenderer3D> ComponentProvider<StaticMeshRenderer3D>::instance_ = ComponentProvider();

namespace {
	mat4 SearchForModelMatrix(SharedPtr<Entity> const &entity) {
		if (entity->hasComponent<Transform>()) {
			return entity->component<Transform>().matrix();
		}
		return entity->root() ? mat4(1.0) : SearchForModelMatrix(entity->parent());
	}
}


bool StaticMeshRenderer3D::culled(RenderPassInfo const &pass_info) {
	std::shared_ptr<Entity> const owner = entity.lock();
	Transform const &transform = owner->component<Transform>();
	for (auto &primitive : mesh->primitives_) {
		if (!primitive.aabb_.onFrustum(pass_info.camera, transform)) {
			wasMostRecentlyCulled = true;
			return true;
		}
	}
	wasMostRecentlyCulled = false;
	return false;
}

void StaticMeshRenderer3D::draw(RenderPassInfo const &pass_info) {
	std::shared_ptr<Entity> const owner = entity.lock();
	mat4 model = SearchForModelMatrix(owner);
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(model));
	gpu_check;
	if (owner->hasComponent<BoneMap>()) {
		BoneMap const &bone_map = owner->component<BoneMap>();
		//bone_map.updateBuffer();
		bone_map.bindBuffer();
	}
	glUniform1i(10, owner->debug_hovered_ ? 1 : 0);
	gpu_check;
	if (pass_info.frustum_culling) {
		primitives_drawn_ = 0;
		for (size_t i = 0; i < mesh->primitives_.size(); i++) {
			auto primitive = mesh->primitives_[i];
			if (!primitive.aabb_.onFrustum(pass_info.camera, owner->component<Transform>()))
				continue;
			primitives_drawn_++;
			mesh->drawSubMesh(pass_info, i);
		}
	}
	else {
		mesh->drawAllSubMeshes(pass_info);
	}
}

#ifdef _DEBUG
void StaticMeshRenderer3D::editor() {
	ImGui::Text("%llu primitives", mesh->primitives_.size());
	ImGui::Text("%i primitives drawn", primitives_drawn_);
}
#endif
