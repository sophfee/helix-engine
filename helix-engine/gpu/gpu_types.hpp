#pragma once

#include "types.hpp"

struct GpuMesh {
	vec3 localBoundsMin;
	u32 materialId;
	vec3 localBoundsMax;
	u32 instanceCount;
	i32 vertexCount;
	u32 indexCount;
	i32 vertexOffset;
	u32 indexOffset;
};

struct GpuMeshTransform {
	mat4 model;
	mat4 view;
	mat4 proj;
	mat4 projView;
	mat4 normal;
};

struct GpuMeshInstance {
	u32 meshId;
	u32 meshTransformId;
};

struct ModelAllocation {
	std::size_t offset, size, generation;
};