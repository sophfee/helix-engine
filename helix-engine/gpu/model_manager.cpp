#include "model_manager.hpp"

#include "ecs/mesh-renderer.h"
#include "ecs/transform.h"
#include "engine/filesystem.hpp"

ModelManager::ModelManager() {
	using namespace gl;
	using enum BufferTargetARB;
	using enum BufferStorageMask;

	draw_command_buffer_.allocateElements(8192, nullptr, DynamicStorageBit | MapReadBit | MapPersistentBit | MapCoherentBit);
	draw_command_count_buffer_.allocateElements(8192, DynamicStorageBit | MapWriteBit | MapPersistentBit | MapCoherentBit);

	draw_command_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 1);
	draw_command_count_buffer_.bindToBackedBufferBlock(ShaderStorageBuffer, 2);

	draw_command_count_buffer_.mapElementsRange(0, 8192, MapBufferAccessMask::MapWriteBit | MapBufferAccessMask::MapPersistentBit | MapBufferAccessMask::MapCoherentBit);

	mesh_buffer_.allocateElements(8192, DynamicStorageBit | MapWriteBit | MapPersistentBit);
	mesh_instance_buffer_.allocateElements(8192, nullptr, DynamicStorageBit | MapWriteBit | MapPersistentBit);
	mesh_transform_buffer_.allocateElements(8192, nullptr, DynamicStorageBit | MapWriteBit | MapPersistentBit);
	
	mesh_buffer_.mapElementsRange(0, 8192,					MapBufferAccessMask::MapWriteBit | MapBufferAccessMask::MapPersistentBit | MapBufferAccessMask::MapFlushExplicitBit);
	mesh_instance_buffer_.mapElementsRange(0, 8192,		MapBufferAccessMask::MapWriteBit | MapBufferAccessMask::MapPersistentBit | MapBufferAccessMask::MapFlushExplicitBit);
	mesh_transform_buffer_.mapElementsRange(0, 8192,	MapBufferAccessMask::MapWriteBit | MapBufferAccessMask::MapPersistentBit | MapBufferAccessMask::MapFlushExplicitBit);
}
ModelManager * ModelManager::singleton() {
	static thread_local ModelManager singleton_;
	return &singleton_;
}
u32 ModelManager::addMesh(SharedPtr<Entity> ent) {
	StaticMeshRenderer3D const &mesh_renderer = ent->component<StaticMeshRenderer3D>();
	Transform const &transform = ent->component<Transform>();
	Mesh const &mesh = *mesh_renderer.mesh;

	const u32 offset = mesh_count;
	const u32 transform_index = ++transforms;
	
	GpuMesh *const gpu_meshes = static_cast<GpuMesh *>(mesh_buffer_.mapped_address_);
	GpuMeshInstance *const gpu_mesh_instances = static_cast<GpuMeshInstance *>(mesh_instance_buffer_.mapped_address_);
	GpuMeshTransform *const gpu_mesh_transforms = static_cast<GpuMeshTransform *>(mesh_transform_buffer_.mapped_address_);

	const mat4 model_matrix = transform.matrix();
	
	gpu_mesh_transforms[transform_index] = GpuMeshTransform{
		.model = model_matrix,
		.inverseModel = glm::inverse(model_matrix)
	};
	
	for (u32 u = mesh_count; u < mesh_count + mesh.meshes.size(); u++) {
		gpu_meshes[u] = mesh.meshes[u - mesh_count];
		gpu_mesh_instances[u] = GpuMeshInstance{
			.meshId = u,
			.meshTransformId = transform_index
		};
	}
	
	mesh_buffer_.flushMappedElementsRange(offset, mesh_count - offset);
	mesh_instance_buffer_.flushMappedElementsRange(offset, mesh_count - offset);
	mesh_transform_buffer_.flushMappedElementsRange(transform_index - 1, 1);
	
	mesh_count += mesh.meshes.size();
	
	mesh.draw_command_offset = offset;
	mesh.draw_command_count_offset = transform_index;
	
	return offset;
}

void ModelManager::prerender() const {
	using namespace gl;
	using enum BufferTargetARB;
	draw_command_buffer_.bindToBackedBufferBlockRange(ShaderStorageBuffer, 1, 0, sizeof(DrawElementsIndirectCommand) * 8192);
	draw_command_count_buffer_.bindToBackedBufferBlockRange(ShaderStorageBuffer, 2, 0, sizeof(u32) * 8192);
	mesh_buffer_.bindToBackedBufferBlockRange(ShaderStorageBuffer, 5, 0, sizeof(GpuMesh) * 8192);
	mesh_instance_buffer_.bindToBackedBufferBlockRange(ShaderStorageBuffer, 6, 0, sizeof(GpuMeshInstance) * 8192);
	mesh_transform_buffer_.bindToBackedBufferBlockRange(ShaderStorageBuffer, 7, 0, sizeof(GpuMeshTransform) * 8192);
}

u32 ModelManager::meshInstanceCount() const {
	return mesh_count;
}

void ModelManager::dispose() {
	draw_command_buffer_.dispose();
	draw_command_count_buffer_.dispose();
	material_buffer_.dispose();
	mesh_buffer_.dispose();
	mesh_instance_buffer_.dispose();
	mesh_transform_buffer_.dispose();
}

bool ModelManager::disposed() const {
	return material_buffer_.disposed() ||
			mesh_buffer_.disposed() ||
			mesh_instance_buffer_.disposed() ||
			mesh_transform_buffer_.disposed();
}