#pragma once

// mesh and attributes n such

#include "types.hpp"
#include "graphics.hpp"
#include <mutex>
#include <type_traits>

#include "gltf.h"

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

#if 0
/**
 * Tightly allocates data to specified attributes at runtime.
 */
class CRuntimeStruct {
private:
	
public:
	CRuntimeStruct();
	CRuntimeStruct(CRuntimeStruct const &other) = delete;
	CRuntimeStruct(CRuntimeStruct &&other) = delete;
	CRuntimeStruct &operator=(CRuntimeStruct const &other) = delete;
	CRuntimeStruct &operator=(CRuntimeStruct &&other) = delete;
	~CRuntimeStruct();

	// Returns field index.
	_NODISCARD _STD size_t defineField(_STD size_t size, _STD size_t padding = 0);
	_NODISCARD _STD size_t getFieldSize(_STD size_t field) const;

	
	
protected:
	void transmogrifyInnerData();
	_NODISCARD bool anythingAllocated() const;
	
private:
	_STD size_t struct_size_;
	_STD size_t struct_alignment_;
	_STD vector<size_t> fields_; //< Each value is the size a field occupies.
	_STD vector<u8> data_;
};
#endif

#pragma pack(push, 1)
struct skinned_vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord0;
	glm::vec3 custom0;
	glm::uint joints0;
	glm::vec4 weights0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct standard_vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord0;
};
#pragma pack(pop)

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
	void processMesh(gltf::data &data, gltf::mesh const &mesh);
	void processMeshAndSkin(gltf::data &data, gltf::mesh &mesh, gltf::skin &skin);
	void processTextures(gltf::data &data);
	void processTexture(gltf::data &data, gltf::texture const &texture);
	void processPrimitiveAttribs(size_t &file_buffer_id, std::fstream &file, gltf::data &data, CSharedPtr<CVertexArray> const &vertex_array, gltf::primitive const &primitive);
	void applyAccessorAsAttribute(size_t &file_buffer_id, std::fstream &file, gltf::data const &data, i32 index, CSharedPtr<CVertexArray> vertex_array, gltf::accessor const &accessor);
	void applyAccessorAsAttributeSingleBuffer(size_t &file_buffer_id, std::fstream &file, std::vector<skinned_vertex> &buffer, size_t offset, gltf::data const &data, i32 index, CSharedPtr<CVertexArray> const &vertex_array, gltf::accessor const &accessor);
	template <typename T> static void applyAccessorAsAttributeSingleBufferUnskinned(size_t &file_buffer_id, std::fstream &file, std::vector<T> &buffer, size_t offset, gltf::data const &data, i32 index, CSharedPtr<CVertexArray> const &vertex_array, gltf::accessor const &accessor);
	void applyAccessorAsElementBuffer(size_t &file_buffer_id, std::fstream &file, gltf::data const &data, CSharedPtr<CVertexArray> vertex_array, gltf::accessor const &accessor);
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

template <typename T>
concept SkinnedVertex = requires(T a)
{
	(a.joints0) -> _STD template convertible_to<glm::uint>;
	(a.joints0) -> _STD template convertible_to<glm::uint>;
};

template <typename T>
void CMeshResource::applyAccessorAsAttributeSingleBufferUnskinned(
	size_t &file_buffer_id,
	std::fstream &file,
	std::vector<T> &buffer,
	size_t offset,
	gltf::data const &data,
	i32 index,
	CSharedPtr<CVertexArray> const &vertex_array,
	gltf::accessor const &accessor)
{
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];

	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

	u64 const attribute_element_size = gltf::sizeForComponentType(accessor.componentType()) * gltf::componentsForType(accessor.type());
	u64 const attribute_buffer_size = buffer_view.length;
	auto const raw_data = &gltf_buffer.data()[buffer_view.offset];
	_STD size_t buffer_capacity = buffer.size() * 64;
	for (size_t i = 0; i < accessor.count(); ++i) {
		switch (index) {
			case 0:
				buffer[i].position = reinterpret_cast<glm::vec3 const *>(raw_data)[i];
				break;
			case 1:
				buffer[i].normal = reinterpret_cast<glm::vec3 const *>(raw_data)[i];
				break;
			case 2:
				buffer[i].texcoord0 = reinterpret_cast<glm::vec2 const *>(raw_data)[i];
				break;
			case 3:
				//buffer[i].custom0 = reinterpret_cast<glm::vec3 const *>(raw_data)[i];
				break;
			case 4:
				//buffer[i].joints0 = reinterpret_cast<uint32_t const *>(raw_data)[i];
				break;
			case 5:
				//buffer[i].weights0 = reinterpret_cast<glm::vec4 const *>(raw_data)[i];
				break;
			default:
				break;
		}
	}
	VertexAttribute_t attrib{};
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
