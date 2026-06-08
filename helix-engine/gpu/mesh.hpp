#pragma once

// mesh and attributes n such

#include "types.hpp"
#include "graphics.hpp"
#include <mutex>
#include <type_traits>

#include "buffer.h"
#include "geometry.hpp"
#include "gltf.h"
class Material;
struct AABB;
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

struct PrimAttribResult;

#pragma pack(push, 1)
struct skinned_vertex {
	vec3 position;
	vec3 normal;
	vec2 texcoord0;
	vec3 custom0;
	glm::uint joints0;
	vec4 weights0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct StandardVertex {
	vec3 position;
	vec3 normal;
	vec2 texcoord0;
	vec4 tangent;
	vec2 texcoord1;
	vec2 unused;
};
#pragma pack(pop)

static_assert(sizeof(StandardVertex) == 64);

class CSkin {
public:
	CSkin();
	CSkin(CSkin &&skin) = delete;
	CSkin(CSkin const& skin) = delete;
	CSkin& operator=(CSkin &&skin) = delete;
	CSkin& operator=(CSkin const& skin) = delete;
	~CSkin();
	
	Vec<mat4> inverse_bind_matrices_;
	Buffer shader_storage_buffer_;
};

class Mesh {
public:
	Mesh();
	Mesh(gltf::data const &data); //< Loads all meshes under one umbrella.
	Mesh(gltf::data const &data, _STD size_t mesh_id); //< loads a specific mesh.
	Mesh(gltf::data &data, _STD size_t mesh_id, Vec<SharedPtr<Buffer>> &views); //< loads a specific mesh.
	Mesh(gltf::data &data, _STD size_t mesh_id, _STD size_t skin_id); //< loads a specific mesh.
	~Mesh();

	Mesh(Mesh const &) = delete;
	Mesh& operator=(Mesh const &) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;

	_NODISCARD _STD size_t subMeshCount() const;

	void drawSubMesh(RenderPassInfo const &info, _STD size_t submesh) const;
	void drawAllSubMeshes(RenderPassInfo const &info) const;

	_NODISCARD bool skinned() const;

private:
	
	void processMesh(gltf::data &data, gltf::mesh const &mesh, Vec<SharedPtr<Buffer>> &views);
	_NODISCARD static AABB processAABB(Vec<StandardVertex> const &vertices);
	void processMeshAndSkin(gltf::data &data, gltf::mesh &mesh, gltf::skin &skin);
	_NODISCARD PrimAttribResult processPrimitiveAttribs(
		gltf::data &data,
		SharedPtr<VertexArray> const &vertex_array,
		gltf::primitive const &primitive,
		Vec<SharedPtr<Buffer>> &views
	);
	void applyAccessorAsAttribute(gltf::data const &data, i32 index, SharedPtr<VertexArray> const &vertex_array, gltf::accessor const &accessor, Vec<SharedPtr<Buffer>> &views);
	void applyAccessorAsAttributeSingleBuffer(size_t &file_buffer_id, std::fstream &file, std::vector<skinned_vertex> &buffer, size_t offset, gltf::data const &data, i32 index, SharedPtr<VertexArray> const &vertex_array, gltf::accessor const &accessor);
	template <typename T> static void applyAccessorAsAttributeSingleBufferUnskinned(size_t &file_buffer_id, std::fstream &file, std::vector<T> &buffer, size_t offset, gltf::data const &data, i32 index, SharedPtr<VertexArray> const &vertex_array, gltf::accessor const &accessor);
	void applyAccessorAsElementBuffer(gltf::data const &data, SharedPtr<VertexArray> const &vertex_array, gltf::accessor const &accessor,
	                                  Vec<SharedPtr<Buffer>> &views);
#ifdef _DEBUG
public:
#else
private:
#endif
	bool is_skinned_;
	struct MeshPrimitive {
		SharedPtr<VertexArray> vertex_array;
		SharedPtr<Material> material;
		AABB aabb_;
	};
	Vec<SharedPtr<Buffer>> buffers_;
	Vec<MeshPrimitive> primitives_;
	Vec<std::future<void>> async_tasks_;
	Optional<CSkin> skin_;
	_STD mutex textures_lock_;

	friend class CSkin;
};

template <typename T>
concept SkinnedVertex = requires(T a)
{
	(a.joints0) -> _STD template convertible_to<glm::uint>;
	(a.joints0) -> _STD template convertible_to<glm::uint>;
};

template <typename T>
void Mesh::applyAccessorAsAttributeSingleBufferUnskinned(
	size_t &file_buffer_id,
	std::fstream &file,
	std::vector<T> &buffer,
	size_t offset,
	gltf::data const &data,
	i32 index,
	SharedPtr<VertexArray> const &vertex_array,
	gltf::accessor const &accessor)
{
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];

	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

	size_t const data_offset = buffer_view.offset + accessor.offset();

	auto const raw_data = &gltf_buffer.data()[data_offset];
	for (size_t i = 0; i < accessor.count(); ++i) {
		switch (index) {
			case 0:
				buffer[i].position  = reinterpret_cast<vec3 const *>(raw_data)[i];
				break;
			case 1:
				buffer[i].normal    = reinterpret_cast<vec3 const *>(raw_data)[i];
				break;
			case 2:
				buffer[i].tangent   = reinterpret_cast<vec4 const *>(raw_data)[i];
				break;
			case 3:
				buffer[i].texcoord0 = reinterpret_cast<vec2 const *>(raw_data)[i];
				break;
			case 4:
				buffer[i].texcoord1 = reinterpret_cast<vec2 const *>(raw_data)[i];
				break;
			default:
				break;
		}
	}
	VertexAttribute_t attrib;
	attrib.offset = static_cast<gltf::id>(offset);
	attrib.type = gltf::gpuComponentTypeFromGltfComponentType(accessor.componentType());
	attrib.size = static_cast<gltf::id>(gltf::sizeForComponentType(accessor.componentType()));
	attrib.binding = 0;
	attrib.stride = sizeof(T);
	attrib.normalized = false;
	attrib.index = index;
	// delete[] raw_data;
	vertex_array->setAttribute(attrib);
	gpu_check;
}
