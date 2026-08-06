#include "mesh-renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "imgui.h"
#include "transform.h"
#include "3d/camera.hpp"
#include "gpu/driver.hpp"
#include "gpu/material.hpp"

ComponentProvider<StaticMeshRenderer3D> ComponentProvider<StaticMeshRenderer3D>::instance_ = ComponentProvider();

namespace {
	mat4 searchForModelMatrix(SharedPtr<Entity> const &entity) {
		if (entity->hasComponent<Transform>()) {
			return entity->component<Transform>().matrix();
		}
		return entity->root() ? mat4(1.0) : searchForModelMatrix(entity->parent());
	}
}


StaticMeshRenderer3D::StaticMeshRenderer3D(SharedPtr<SceneTree> const &p_tree, SharedPtr<Entity> const &p_entity): Component(p_tree, p_entity) {
	GraphicsBackend* driver = GraphicsDriver::get();
	
using namespace gfx;
	constexpr BufferDescriptor transform_buffer_desc = {
		.size = sizeof(GpuMeshTransform),
		.usage = BitFlag(BufferUsage::eUniform) | BitFlag(BufferUsage::eShaderDeviceAddress),
		.memory_usage = MemoryUsage::eAuto,
		.allocation_hints = BitFlag(AllocationHint::eMapped) |
			BitFlag(AllocationHint::eHostSequentialWrite) |
			BitFlag(AllocationHint::eAllowTransferInstead)
	};
	
	transform_buffer_ = driver->buffer_create(transform_buffer_desc);
	static const char* name = "TRANSFORM BUFFER";
	driver->buffer_set_name(transform_buffer_, name);
	transform_ = (GpuMeshTransform*)driver->buffer_mapped_data(transform_buffer_);
}

bool StaticMeshRenderer3D::culled(RenderPassInfo const &pass_info) {
	return false;
}

void StaticMeshRenderer3D::draw(RenderPassInfo const &pass_info) {
	std::shared_ptr<Entity> const owner = entity.lock();
	const Transform &transform = owner->component<Transform>();
	const mat4 model = transform.matrix();
	
	Camera3D* camera = Camera3D::currentCameraEntity();
	
	mat4 mvp = camera->projectionViewMatrix() * model;
	
	const GpuMeshTransform updated_transform{
		.model = mvp,
		.inverseModel = glm::inverse(model)
	};
	*transform_ = updated_transform;

	const RID pipeline_layout = pass_info.pipeline_layout;
	const RID cmd = pass_info.cmd;
	
	GraphicsBackend* driver = GraphicsDriver::get();

	const vk::DeviceAddress address = driver->buffer_virtual_address(transform_buffer_);

	const PushConstantRangeDescriptor push_constant_range = {
		.visibility = BitFlag(gfx::ShaderStage::eVertex),
		.offset = 0,
		.size = sizeof(vk::DeviceAddress)
	};
	driver->push_constants(cmd, pipeline_layout, push_constant_range, &address);
	if (!mesh->materials_.empty() && mesh->materials_[0]->bind_group_.lower != 0) {
		driver->set_bind_group(cmd, pipeline_layout, 0, mesh->materials_[0]->bind_group_, gfx::ShaderStage::eFragment);
	}
	mesh->drawAllSubMeshes(pass_info); 
}

void StaticMeshRenderer3D::destroy() {
	Component::destroy();
	GraphicsDriver::get()->buffer_delete(transform_buffer_);
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
}
#endif
