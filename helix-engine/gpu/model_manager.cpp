#include "model_manager.hpp"

#include "ecs/mesh-renderer.h"
#include "ecs/transform.h"

ModelManager::ModelManager() {
	using namespace gl;
	using enum BufferTargetARB;
	using enum BufferStorageMask;
	draw_command_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 1);
	draw_command_buffer_.allocateElements(8192, nullptr, DynamicStorageBit);
	draw_command_count_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 2);
	draw_command_count_buffer_.allocateElement(0, DynamicStorageBit);
	visible_meshes_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 8);
	visible_meshes_buffer_.allocateElement(0, DynamicStorageBit);
}
ModelManager * ModelManager::singleton() {
	static thread_local ModelManager singleton_;
	return &singleton_;
}
void ModelManager::addMesh(SharedPtr<Entity> ent) {
	/* Download all current data */

	/*
	Vec<Vertex> vertices;
	Vec<u16> indices;
	Vec<GpuMesh> meshes;
	Vec<GpuMeshTransform> transforms;
	Vec<GpuMeshInstance> instances;
	
	vertex_buffer_.downloadElements(vertices);
	index_buffer_.downloadElements(indices);
	mesh_buffer_.downloadElements(meshes);
	mesh_transform_buffer_.downloadElements(transforms);
	mesh_instance_buffer_.downloadElements(instances);

	auto const &static_mesh_renderer = ent->component<StaticMeshRenderer3D>();
	Mesh *mesh = static_mesh_renderer.mesh.get();
	Transform const &xform = ent->component<Transform>();
	mat4 const model = xform.matrix();
	mat4 const inv_model = glm::inverse(model);
	transforms.push_back({model, inv_model});

	for (Mesh::MeshPrimitive &primitive : mesh->primitives_) {
		meshes.push_back({primitive.aabb_.min(), 0, primitive.aabb_.max(), 1, static_cast<u32>(primitive.vertices.size()), static_cast<u32>(primitive.indices.size()), vertices.size(), indices.size()});
		vertices.insert(vertices.end(), primitive.vertices.begin(), primitive.vertices.end());
		indices.insert(indices.end(), primitive.indices.begin(), primitive.indices.end());
		instances.push_back({static_cast<u32>(meshes.size() - 1), static_cast<u32>(transforms.size() - 1)});
	}

	vertex_buffer_.recreateElements(vertices);
	index_buffer_.recreateElements(indices);
	mesh_buffer_.recreateElements(meshes);
	mesh_transform_buffer_.recreateElements(transforms);
	mesh_instance_buffer_.recreateElements(instances);
	mesh_instance_count = static_cast<u32>(meshes.size());
	*/
}

void ModelManager::prerender() const {
	using namespace gl;
	using enum BufferTargetARB;
	draw_command_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 1);
	draw_command_count_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 2);
	vertex_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 3);
	index_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 4);
	mesh_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 5);
	mesh_instance_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 6);
	mesh_transform_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 7);
	visible_meshes_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 8);
}

u32 ModelManager::meshInstanceCount() const {
	return mesh_instance_count;
}

void ModelManager::dispose() {
	draw_command_buffer_.dispose();
	draw_command_count_buffer_.dispose();
	vertex_buffer_.dispose();
	index_buffer_.dispose();
	material_buffer_.dispose();
	mesh_buffer_.dispose();
	mesh_instance_buffer_.dispose();
	mesh_transform_buffer_.dispose();
	visible_meshes_buffer_.dispose();
}

bool ModelManager::disposed() const {
	return vertex_buffer_.disposed() ||
			index_buffer_.disposed() ||
			material_buffer_.disposed() ||
			mesh_buffer_.disposed() ||
			mesh_instance_buffer_.disposed() ||
			mesh_transform_buffer_.disposed() ||
			visible_meshes_buffer_.disposed();
}
