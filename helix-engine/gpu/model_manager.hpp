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

	void addMesh(SharedPtr<Entity> ent);
	void prerender() const;
	[[nodiscard]] u32 meshInstanceCount() const;
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

	
// private:

	VertexArray vao_;

	Vec<Mesh*> meshes_;
	u32 mesh_instance_count = 0;
	
	TypedBuffer<DrawArraysIndirectCommand> draw_command_buffer_;
	TypedBuffer<u32> draw_command_count_buffer_;
	
	TypedBuffer<Vertex> vertex_buffer_;
	TypedBuffer<u16> index_buffer_; //< This can become packed u8x4, u16x2, etc
	TypedBuffer<GpuMaterial> material_buffer_;
	
	TypedBuffer<GpuMesh> mesh_buffer_;
	TypedBuffer<GpuMeshInstance> mesh_instance_buffer_;
	TypedBuffer<GpuMeshTransform> mesh_transform_buffer_;
	TypedBuffer<u32> visible_meshes_buffer_;
};