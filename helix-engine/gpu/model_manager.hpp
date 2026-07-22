#pragma once

#include "buffer.h"
#include "gl_structs.h"
#include "gpu_types.hpp"
#include "gpu/mesh.hpp"
#include "material.hpp"

class Entity;


class ModelManager : IDisposable {
private:
	ModelManager();
	
public:
	inline static std::size_t ALLOCATION_STEP = 1 << 20;
	
	static ModelManager *singleton();

	u32 addMesh(SharedPtr<Entity> ent);
	void prerender() const;
	[[nodiscard]] u32 meshInstanceCount() const;
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

	
// private:
	
	TypedBuffer<DrawArraysIndirectCommand> draw_command_buffer_;
	TypedBuffer<gl::sizei_t> draw_command_count_buffer_;
	TypedBuffer<GpuMesh> mesh_buffer_;
	TypedBuffer<GpuMeshInstance> mesh_instance_buffer_;
	TypedBuffer<GpuMeshTransform> mesh_transform_buffer_;
	u32 mesh_count = 0;
	u32 transforms = 0;
	
	TypedBuffer<GpuMaterial> material_buffer_;
	
	
};