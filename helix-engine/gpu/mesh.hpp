#pragma once

// mesh and attributes n such

#include "types.hpp"
#include "graphics.hpp"
#include <mutex>
#include <unordered_map>

namespace gltf {
	struct skin;
	struct primitive;
	struct mesh;
	struct material;
	struct data;
	class accessor;
}

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
	CMeshResource(gltf::data &data); //< Loads all meshes under one umbrella.
	CMeshResource(gltf::data &data, _STD size_t mesh_id); //< loads a specific mesh.
	CMeshResource(gltf::data &data, _STD size_t mesh_id, _STD size_t skin_id); //< loads a specific mesh.
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
	void processMesh(gltf::data &data, gltf::mesh &mesh);
	void processMeshAndSkin(gltf::data &data, gltf::mesh &mesh, gltf::skin &skin);
	void processTextures(gltf::data &data);
	void processPrimitiveAttribs(gltf::data &data, CSharedPtr<CVertexArray> const &vertex_array, gltf::primitive const &primitive);
	void applyAccessorAsAttribute(gltf::data const &data, i32 index, CSharedPtr<CVertexArray> vertex_array, gltf::accessor const &accessor);
	void applyAccessorAsElementBuffer(gltf::data const &data, CSharedPtr<CVertexArray> vertex_array, gltf::accessor const &accessor);
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
	CVector<gltf::material> material_info_;
	CVector<CSharedPtr<CBuffer>> buffers_;
	CVector<CSharedPtr<CTexture>> textures_;
	_STD mutex textures_lock_;
	COptional<CSkin> skin_;

	friend class CSkin;
};