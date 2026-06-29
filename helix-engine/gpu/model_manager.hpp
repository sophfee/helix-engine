#pragma once

#include "buffer.h"
#include "gpu/mesh.hpp"
#include "material.hpp"

class ModelManager : IDisposable {
public:
	static ModelManager *singleton();

	void dispose() override;
	[[nodiscard]] bool disposed() const override;
	
private:
	Vec<Vertex> vertices_;
	Vec<u32> indices_;
	Vec<GpuMaterial> materials_;
	
	TypedBuffer<Vertex> vertex_buffer_;
	TypedBuffer<u32> index_buffer_; //< This can become packed u8x4, u16x2, etc
	TypedBuffer<GpuMaterial> material_buffer_;
};