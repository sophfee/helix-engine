#include "mesh-renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "imgui.h"
#include "transform.h"
#include "3d/camera.hpp"
#include "gpu/driver.hpp"
#include "gpu/material.hpp"
#include "gpu/window.hpp"
#include "gpu/renderers/renderer.hpp"

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
	const BufferDescriptor transform_buffer_desc = {
		.label = "StaticMeshRenderer3D Transform Buffer",
		.size = sizeof(GpuMeshTransform),
		.usage = BufferUsage::eUniform | BufferUsage::eShaderDeviceAddress,
		.memory_usage = MemoryUsage::eAuto,
		.allocation_hints = AllocationHint::eMapped | AllocationHint::eHostSequentialWrite | AllocationHint::eAllowTransferInstead
	};
	transform_buffer_ = driver->buffer_create(transform_buffer_desc);
	transform_ = (GpuMeshTransform*)driver->buffer_mapped_data(transform_buffer_);
}
bool StaticMeshRenderer3D::culled(RenderPassInfo const &pass_info) {
	return false;
}
void StaticMeshRenderer3D::update(double x) {
	if (bind_group_layout.lower == 0) return;
	RID primary_bind_group_layout = window()->renderer()->primaryBindGroupLayout();
	for (auto& prim : mesh->buffers_)
		prim.material->update(primary_bind_group_layout);
}
void StaticMeshRenderer3D::draw(RenderPassInfo const &pass_info) {
	std::shared_ptr<Entity> const owner = entity.lock();
	const Transform &transform = owner->component<Transform>();
	const mat4 model = transform.matrix();
	Camera3D* camera = Camera3D::currentCameraEntity();
	const GpuMeshTransform updated_transform{
		.model = model,
		.view = camera->viewMatrix(), //glm::inverse(model)
		.proj = camera->projectionMatrix(),
		.projView = camera->projectionViewMatrix(),
		.normal = glm::transpose(glm::inverse(camera->viewMatrix() * model))
	};
	*transform_ = updated_transform;
	GraphicsBackend* driver = GraphicsDriver::get();
	const RID pipeline_layout = pass_info.pipeline_layout;
	const RID cmd = pass_info.cmd;
	const vk::DeviceAddress address = driver->buffer_virtual_address(transform_buffer_);
	const PushConstantRangeDescriptor push_constant_range = {
		.visibility = gfx::ShaderStage::eTask |  gfx::ShaderStage::eMesh | gfx::ShaderStage::eFragment,
		.offset = 0,
		.size = sizeof(float4x4)
	};
	driver->push_constants(cmd, pipeline_layout, push_constant_range, &model);
	pass_info.scene_data->normal = glm::inverse(glm::transpose(pass_info.scene_data->view * model));
	mesh->drawAllSubMeshes(pass_info);
}
void StaticMeshRenderer3D::renderSetup(RenderPassInfo const &pass_info) {
	bind_group_layout = pass_info.material_bind_group_layout;
	std::shared_ptr<Entity> const owner = entity.lock();
	for (const auto &buffer : mesh->buffers_) {
		buffer.material->renderSetup(pass_info, *mesh, *owner);
	}
}
void StaticMeshRenderer3D::destroy() {
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
