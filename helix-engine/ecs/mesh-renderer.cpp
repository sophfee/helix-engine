#include "mesh-renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "bone-map.h"
#include "imgui.h"
#include "transform.h"
#include "gpu/material.hpp"
#include "gpu/texture.h"

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
	gpu_check;
	std::shared_ptr<Entity> const owner = entity.lock();
	mat4 const model = SearchForModelMatrix(owner);
	if (pass_info.bind_model_matrix && pass_info.model_matrix_location != -1)
		pass_info.shader_program->setUniform(pass_info.model_matrix_location, model);

	if (pass_info.bind_debug_hovered && pass_info.debug_hovered_location != -1)
		pass_info.shader_program->setUniform(pass_info.debug_hovered_location, owner->debug_hovered_ ? 1 : 0);

	/*
	if (pass_info.frustum_culling) {
		Transform const &transform = owner->component<Transform>();
		primitives_drawn_ = 0;
		for (size_t i = 0; i < mesh->primitives_.size(); i++) {
			if (auto primitive = mesh->primitives_[i];
				primitive.aabb_.onFrustum(pass_info.camera, transform)) {
				primitives_drawn_++;
				mesh->drawSubMesh(pass_info, i);
			}
		}
	}
	else {
	*/
	mesh->drawAllSubMeshes(pass_info);
	//}
	gpu_check;
}


#ifdef _DEBUG

class ImIndentation {
public:
	ImIndentation() {
		ImGui::Indent();
	}
	~ImIndentation() {
		ImGui::Unindent();
	}
};

void StaticMeshRenderer3D::editor() {
	using namespace ImGui;

	if (Button("Mesh Inspector")) {
		open_inspector = true;
	}

	if (open_inspector) {
		std::string const name = "Mesh Inspector - " + entity.lock()->name_;
		if (Begin(name.c_str(), &open_inspector)) {
			int primitive_id = 0;
			for (Mesh::MeshPrimitive const &primitive : mesh->primitives_) {
				if (CollapsingHeader(("Primitive " + std::to_string(primitive_id++)).c_str(), ImGuiTreeNodeFlags_FramePadding)) {
					ImIndentation indent;

					if (primitive.material) {
						SharedPtr<Material> material = primitive.material;

						ColorEdit4("Emissive", &material->emissive_color_mod_[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
					}
				}
			}
		}
		End();
	}
}
#endif
