// ReSharper disable CppClangTidyClangDiagnosticPadded
#pragma once

// mesh and attributes n such

#include "types.hpp"
#include "graphics.hpp"
#include <mutex>

#include "geometry.hpp"
#include "gltf.h"
#include "gpu_types.hpp"
#include "material.hpp"
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

struct Vertex {
	alignas(16)
	vec3 position;
	alignas(16)
	vec3 normal;
	alignas(16)
	vec4 tangent;
	alignas(8)
	vec2 texcoord0;
	alignas(8)
	vec2 texcoord1;

	static vk::PipelineVertexInputStateCreateInfo inputState() {
		static std::array inputAttributeDescriptions = {
			vk::VertexInputAttributeDescription()
				.setBinding(0)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setLocation(0)
				.setOffset(0),
			vk::VertexInputAttributeDescription()
				.setBinding(0)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setLocation(1)
				.setOffset(offsetof(Vertex, normal)),
			vk::VertexInputAttributeDescription()
				.setBinding(0)
				.setFormat(vk::Format::eR32G32B32A32Sfloat)
				.setLocation(2)
				.setOffset(offsetof(Vertex, tangent)),
			vk::VertexInputAttributeDescription()
				.setBinding(0)
				.setFormat(vk::Format::eR32G32Sfloat)
				.setLocation(3)
				.setOffset(offsetof(Vertex, texcoord0)),
			vk::VertexInputAttributeDescription()
				.setBinding(0)
				.setFormat(vk::Format::eR32G32Sfloat)
				.setLocation(4)
				.setOffset(offsetof(Vertex, texcoord1))
		};
		
		static std::array inputBindingDescription = {
			vk::VertexInputBindingDescription()
				.setBinding(0)
				.setInputRate(vk::VertexInputRate::eVertex)
				.setStride(sizeof(Vertex))
		};
		
		static vk::PipelineVertexInputStateCreateInfo vertexInputState = vk::PipelineVertexInputStateCreateInfo()
			.setVertexAttributeDescriptions(inputAttributeDescriptions)
			.setVertexBindingDescriptions(inputBindingDescription);
		
		return vertexInputState;
	}
};

static_assert(sizeof(Vertex) == 64);


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
	void drawSubMesh(RenderPassInfo const &info, _STD size_t submesh);
	void drawAllSubMeshes(RenderPassInfo const &info);
	void setMaterial(std::size_t index, SharedPtr<Material> const &material);
private:
	
	void processMesh(gltf::data &data, gltf::mesh const &mesh, Vec<SharedPtr<Buffer>> &views);
	_NODISCARD static AABB processAABB(Vec<Vertex> const &vertices);
	void processMeshAndSkin(gltf::data &data, gltf::mesh &mesh, gltf::skin &skin);;

	_NODISCARD static AABB processPrimitiveAttribsIntoVertexVector(
		gltf::data &data,
		gltf::primitive const &primitive,
		Vec<Vertex> &out_vertices
	);

	_NODISCARD GpuMesh processPrimitiveAttribsIntoSeparateVector(
		gltf::data &data,
		gltf::primitive const &primitive,
		Vec<vec3> &position_vector,
		Vec<vec3> &normal_vector,
		Vec<vec4> &tangent_vector,
		Vec<vec2> &texcoord0_vector,
		Vec<vec2> &texcoord1_vector
	);
	
#ifdef _DEBUG
public:
#else
private:
#endif

	// TODO: Separate the Mesh class into subclasses that use Mesh as a container wrapper. Separate by different renderers needs. 
	bool is_skinned_;

	Vec<SharedPtr<Material>> materials_;
	
	RID buffer_;
	VkDeviceSize vertex_buffer_size_;
	VkDeviceSize index_count_;

	mutable u32 draw_command_offset;
	mutable u32 draw_command_count_offset;

	Vec<GpuMesh> meshes;
	
	struct MeshPrimitive {
		SharedPtr<Material> material;
		u32 vertex_count;
		u32 index_count;
		AABB aabb_;
	};

	u32 mesh_count = 0;
	
	Vec<MeshPrimitive> primitives_;
	Vec<std::future<void>> async_tasks_;
	_STD mutex textures_lock_;
	
	Vec<SharedPtr<Buffer>> buffers_;
};