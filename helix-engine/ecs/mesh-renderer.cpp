#include "mesh-renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "transform.h"
#include "gpu/driver.hpp"
#include "gpu/material.hpp"

ComponentProvider<StaticMeshRenderer3D> ComponentProvider<StaticMeshRenderer3D>::instance_ = ComponentProvider();

namespace {
	mat4 SearchForModelMatrix(SharedPtr<Entity> const &entity) {
		if (entity->hasComponent<Transform>()) {
			return entity->component<Transform>().matrix();
		}
		return entity->root() ? mat4(1.0) : SearchForModelMatrix(entity->parent());
	}
}


StaticMeshRenderer3D::StaticMeshRenderer3D(SharedPtr<SceneTree> const &p_tree, SharedPtr<Entity> const &p_entity): Component(p_tree, p_entity) {
	GraphicsDriver* driver = GraphicsDriver::singleton();
	
	vk::BufferCreateInfo transformBufferCreateInfo = vk::BufferCreateInfo()
		.setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress)
		.setSize(sizeof(GpuMeshTransform));
	
	VmaAllocationCreateInfo allocationCreateInfo = {
		.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO
	};
	
	transform_buffer_ = driver->buffer_create(transformBufferCreateInfo, allocationCreateInfo);
	static const char* name = "TRANSFORM BUFFER";
	driver->buffer_set_allocation_name(transform_buffer_, name);
	transform_ = (GpuMeshTransform*)driver->buffer_get_mapped_address(transform_buffer_);
}

bool StaticMeshRenderer3D::culled(RenderPassInfo const &pass_info) {
	return false;
}

void StaticMeshRenderer3D::draw(RenderPassInfo const &pass_info) {
	std::shared_ptr<Entity> const owner = entity.lock();
	const Transform &transform = owner->component<Transform>();
	const glm::mat4 model = transform.matrix();
	const GpuMeshTransform updated_transform{
		.model = model,
		.inverseModel = glm::inverse(model)
	};
	*transform_ = updated_transform;

	const RID pipeline = pass_info.pipeline;
	const RID cmd = pass_info.cmd;
	
	GraphicsDriver* driver = GraphicsDriver::singleton();
	
	vk::DeviceAddress address = driver->buffer_get_device_address(transform_buffer_);
	driver->push_constants(cmd, pipeline, vk::ShaderStageFlagBits::eVertex, 0, sizeof(vk::DeviceAddress), &address);
	mesh->drawAllSubMeshes(pass_info); 
}

void StaticMeshRenderer3D::destroy() {
	GraphicsDriver::singleton()->buffer_delete(transform_buffer_);
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

						ColorEdit4("Diffuse Modulation", &material->diffuse_modulation_[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
						SliderFloat("Roughness", &material->roughness_, 0.0f, 1.0f);
						SliderFloat("Metallic", &material->metallic_, 0.0f, 1.0f);
						ColorEdit4("Emissive", &material->emissive_color_mod_[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
					}
				}
			}
		}
		End();
	}
}
#endif
