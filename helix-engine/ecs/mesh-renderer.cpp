#include "mesh-renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "bone-map.h"
#include "imgui.h"
#include "transform.h"
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
	std::shared_ptr<Entity> const owner = entity.lock();
	mat4 model = SearchForModelMatrix(owner);
	if (pass_info.bind_model_matrix) {
		glUniformMatrix4fv(pass_info.model_matrix_location, 1, GL_FALSE, glm::value_ptr(model));
		gpu_check;
	}
	if (pass_info.bind_debug_hovered) {
		glUniform1i(pass_info.debug_hovered_location, owner->debug_hovered_ ? 1 : 0);
		gpu_check;
	}

	if (pass_info.bind_object_id)
		glUniform1ui(11, owner->id());

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
		mesh->drawAllSubMeshes(pass_info);
	}
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
			for (Mesh::Primitive_t const &primitive : mesh->primitives_) {
				if (CollapsingHeader(("Primitive " + std::to_string(primitive_id++)).c_str(), ImGuiTreeNodeFlags_FramePadding)) {
					ImIndentation indent;
					if (CollapsingHeader("Material")) {
						ImIndentation material_indent;
						auto const &material_info = mesh->material_info_[primitive.material];
						if (CollapsingHeader("Base Color")) {
							ImIndentation color_indent;
							ColorEdit4("Base Color Factor", (float*)glm::value_ptr(material_info.pbr_metallic_roughness.base_color_factor));
							if (material_info.pbr_metallic_roughness.base_color_texture.index != -1) {
								auto const &tex = mesh->textures_[material_info.pbr_metallic_roughness.base_color_texture.index];
								tex->inspector();
							}
						}
						if (material_info.normal_texture.index != -1) {
							if (CollapsingHeader("Normal Map")) {
								ImIndentation normal_indent;
								auto const &tex = mesh->textures_[material_info.normal_texture.index];
								tex->inspector();
							}
						}

						if (CollapsingHeader("PBR")) {
							if (material_info.pbr_metallic_roughness.metallic_roughness_texture.index != -1) {
								ImIndentation pbr_indent;
								SliderFloat("Metallic Factor", (float*)&(material_info.pbr_metallic_roughness.metallic_factor), 0.0f, 1.0f);
								SliderFloat("Roughness Factor", (float*)&(material_info.pbr_metallic_roughness.roughness_factor), 0.0f, 1.0f);
								if (CollapsingHeader("Metallic Roughness Map")) {
									ImIndentation metal_indent;
									SharedPtr<Texture> const &tex = mesh->textures_[material_info.pbr_metallic_roughness.metallic_roughness_texture.index];
									tex->inspector();
								}
							}
						}
					}
				}
			}
		}
		End();
	}
}
#endif
