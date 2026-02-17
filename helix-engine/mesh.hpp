#pragma once

// mesh and attributes n such

#include "types.hpp"
#include "graphics.hpp"
#include <mutex>

class CGltfAccessor;
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

	[[nodiscard]] std::size_t subMeshCount() const;

	void drawSubMesh(std::size_t const submesh) const;
	void drawAllSubMeshes() const;

private:
	void applyAccessorAsAttribute(GltfData_t const &data, i32 index, std::shared_ptr<CVertexArray> vertex_array, CGltfAccessor const &accessor);
	void applyAccessorAsElementBuffer(GltfData_t const &data, std::shared_ptr<CVertexArray> vertex_array, CGltfAccessor const &accessor);
#ifdef _DEBUG
public:
#else
private:
#endif
	std::vector<std::shared_ptr<CVertexArray>> vertex_arrays_;
	std::vector<std::shared_ptr<CBuffer>> buffers_;
	std::mutex textures_lock_;
	std::vector<std::shared_ptr<CTexture>> textures_;
};