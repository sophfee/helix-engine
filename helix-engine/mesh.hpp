#pragma once

// mesh and attributes n such

#include "types.hpp"
#include "graphics.hpp"

struct GltfData_t;

constexpr static VertexAttribute_t GenericPositionAttribute{
	.index = 0,
	.binding = 0,
	.size = 3,
	.stride = 0,
	.offset = 0,
	.type = EComponentType::SINGLE_FLOAT,
	.normalized = false
};

constexpr static VertexAttribute_t GenericNormalAttribute{
	.index = 1,
	.binding = 1,
	.size = 3,
	.stride = 0,
	.offset = 0,
	.type = EComponentType::SINGLE_FLOAT,
	.normalized = true
};

constexpr static VertexAttribute_t GenericTexCoordAttribute{
	.index = 2,
	.binding = 2,
	.size = 2,
	.stride = 0,
	.offset = 0,
	.type = EComponentType::SINGLE_FLOAT,
	.normalized = false
};

class CMesh {
public:
	CMesh();
	CMesh(GltfData_t &data);
	~CMesh();

	CMesh(CMesh const &) = delete;
	CMesh& operator=(CMesh const &) = delete;
	CMesh(CMesh&&) = delete;
	CMesh& operator=(CMesh&&) = delete;
	
private:
	std::vector<CVertexArray> vertex_arrays_;
};