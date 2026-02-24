#pragma once

// mesh and attributes n such

#include "types.hpp"
#include "graphics.hpp"
#include <mutex>
#include <unordered_map>

struct GltfSkin_t;
struct GltfMeshPrimitive_t;
struct GltfMesh_t;
struct GltfMaterial_t;
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

class CSkin {
public:
	CSkin();
	CSkin(CSkin &&skin) = delete;
	CSkin(CSkin const& skin) = delete;
	CSkin& operator=(CSkin &&skin) = delete;
	CSkin& operator=(CSkin const& skin) = delete;
	~CSkin();
	
	CVector<glm::mat4> inverse_bind_matrices_;
	CBuffer shader_storage_buffer_;
};

class CMeshResource {
public:
	CMeshResource();
	CMeshResource(GltfData_t &data); //< Loads all meshes under one umbrella.
	CMeshResource(GltfData_t &data, _STD size_t mesh_id); //< loads a specific mesh.
	CMeshResource(GltfData_t &data, _STD size_t mesh_id, _STD size_t skin_id); //< loads a specific mesh.
	~CMeshResource();

	CMeshResource(CMeshResource const &) = delete;
	CMeshResource& operator=(CMeshResource const &) = delete;
	CMeshResource(CMeshResource&&) = delete;
	CMeshResource& operator=(CMeshResource&&) = delete;

	_NODISCARD _STD size_t subMeshCount() const;

	void drawSubMesh(_STD size_t submesh) const;
	void drawAllSubMeshes() const;

	_NODISCARD bool skinned() const;

private:
	void processMesh(GltfData_t &data, GltfMesh_t &mesh);
	void processMeshAndSkin(GltfData_t &data, GltfMesh_t &mesh, GltfSkin_t &skin);
	void processTextures(GltfData_t &data);
	void processPrimitiveAttribs(GltfData_t &data, CSharedPtr<CVertexArray> const &vertex_array, GltfMeshPrimitive_t const &primitive);
	void applyAccessorAsAttribute(GltfData_t const &data, i32 index, CSharedPtr<CVertexArray> vertex_array, CGltfAccessor const &accessor);
	void applyAccessorAsElementBuffer(GltfData_t const &data, CSharedPtr<CVertexArray> vertex_array, CGltfAccessor const &accessor);
#ifdef _DEBUG
public:
#else
private:
#endif
	bool is_skinned_;
	struct Primitive_t {
		CSharedPtr<CVertexArray> vertex_array;
		u32 material = 0;
	};
	CVector<Primitive_t> primitives_;
	CVector<GltfMaterial_t> material_info_;
	CVector<CSharedPtr<CBuffer>> buffers_;
	CVector<CSharedPtr<CTexture>> textures_;
	_STD mutex textures_lock_;
	COptional<CSkin> skin_;

	friend class CSkin;
};