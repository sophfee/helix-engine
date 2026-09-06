#include "static_mesh_renderer.h"
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
	mat4 searchForModelMatrix(Entity* entity) {
		if (entity->has_component<Transform>())
			return entity->get_component<Transform>().get_matrix();
		return entity->is_root() ? mat4(1.0) : searchForModelMatrix(entity->get_parent());
	}
}

StaticMeshRenderer3D::StaticMeshRenderer3D() : mesh(nullptr), transform_(nullptr), transform_buffer_(0) {
}

StaticMeshRenderer3D::StaticMeshRenderer3D(SharedPtr<SceneTree> const &p_tree, const RID p_entity): Component(p_tree, p_entity) {
	IGpuDriver* driver = GraphicsSystem::get_driver();
	using namespace gfx;
	const BufferDescriptor transform_buffer_desc = {
		.label = "StaticMeshRenderer3D Transform Buffer",
		.size = sizeof(PerModelData) * 2,
		.usage = BufferUsage::eUniform | BufferUsage::eShaderDeviceAddress,
		.memory_usage = MemoryUsage::ePreferDevice,
		.allocation_hints = AllocationHint::eHostAccessRandom | AllocationHint::eMapped
	};
	transform_buffer_ = driver->create_buffer(transform_buffer_desc);
	driver->set_buffer_name(transform_buffer_, "StaticMeshRenderer3D Transform Buffer");
	transform_ = (PerModelData*)driver->get_mapped_data(transform_buffer_);
}


bool StaticMeshRenderer3D::culled(RenderPassInfo const &pass_info) {
	return false;
}
void StaticMeshRenderer3D::update(double x) {
	if (bind_group_layout.lower == 0) return;
	RID primary_bind_group_layout = get_window()->get_renderer()->get_primary_bind_group_layout();
	for (const Mesh::Primitive &prim : mesh->buffers_)
		prim.material->update(primary_bind_group_layout);
}
void StaticMeshRenderer3D::draw(RenderPassInfo const &pass_info) {
	if (mesh->get_sub_mesh_count() <= 0) return;
	
	const Entity *owner = get_entity();
	const Transform &transform = owner->get_component<Transform>();
	const mat4 model = transform.get_matrix();
	const Camera3D *camera = Camera3D::get_current_camera_entity();
	IGpuDriver *driver = GraphicsSystem::get_driver();
	
	const float4x4 normal = glm::inverse(glm::transpose(pass_info.view * model));
	const PerModelData updated_transform{
		.model = model,
		.normal = normal
	};
	
	if (transform.dirty_[pass_info.frame_index] || transform_[pass_info.frame_index].normal != updated_transform.normal) {
		transform_[pass_info.frame_index] = updated_transform;
		transform.dirty_[pass_info.frame_index] = false;
		just_cleaned = true;
	}
	
	const RID pipeline_layout = pass_info.pipeline_layout;
	const RID cmd = pass_info.cmd;
	const vk::DeviceAddress address = driver->get_buffer_virtual_address(transform_buffer_) + sizeof(PerModelData) * pass_info.frame_index;
	constexpr PushConstantRangeDescriptor push_constant_range = {
		.visibility = gfx::ShaderStage::eVertex | gfx::ShaderStage::eFragment,
		.offset = sizeof(GpuDeviceAddress),
		.size = sizeof(GpuDeviceAddress)
	};
	driver->push_constants(cmd, pipeline_layout, push_constant_range, &address);
	mesh->draw_all_sub_meshes(pass_info);
}

void StaticMeshRenderer3D::render_setup(RenderPassInfo const &pass_info) {
	bind_group_layout = pass_info.material_bind_group_layout;
	const Entity *owner = get_entity();
	for (const Mesh::Primitive &buffer : mesh->buffers_)
		buffer.material->render_setup(pass_info, *mesh, *owner);
}

void StaticMeshRenderer3D::destroy() {
	GraphicsSystem::get_driver()->destroy_buffer(transform_buffer_);
	mesh.reset();
}

#ifdef _DEBUG


void StaticMeshRenderer3D::editor() {
	ImGui::Spacing();
	ImGui::SeparatorText("Component: StaticMeshRenderer3D");
	ImGui::Text("Was just cleaned? %d", just_cleaned ? 1 : 0);
	
	GraphicsSystem::get_driver()->imgui_draw_buffer_resource_info(transform_buffer_);
	
	just_cleaned = false;
} 
#endif
