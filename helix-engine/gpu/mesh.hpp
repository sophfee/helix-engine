// ReSharper disable CppClangTidyClangDiagnosticPadded
#pragma once

// mesh and attributes n such

#include "types.hpp"
#include "graphics.hpp"

#include "geometry.hpp"
#include "gltf.h"
#include "gpu_types.hpp"
#include "material.hpp"
#include "meshoptimizer.h"
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
	float3 position;
	float3 normal;
	float2 texcoord0;
	float3 custom0;
	glm::uint joints0;
	float4 weights0;
};
#pragma pack(pop)

struct Vertex {
	alignas(16)
	float3 position;
	// alignas(16)
	alignas(16)
	float3 normal;
	// alignas(16)
	alignas(16)
	float4 tangent;
	// alignas(8)
	alignas(16)
	float2 texcoord0;
	// alignas(8)
	//float2 texcoord1;

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
				.setOffset(offsetof(Vertex, texcoord0))
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

//static_assert(sizeof(Vertex) == 64);

struct Meshlet {
	float4 bounding_sphere;
	
	float3 cone_apex;
	float cutoff;
	
	float3 cone_axis;
	uint32_t vertex_offset;
	
	uint32_t meshlet_vertices_offset;
	uint32_t meshlet_triangle_offset;
	uint32_t meshlet_vertices_count;
	uint32_t meshlet_triangle_count;
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
	void drawSubMesh(RenderPassInfo const &info, _STD size_t submesh);
	void drawAllSubMeshes(RenderPassInfo const &info);
	void setMaterial(std::size_t index, SharedPtr<Material> const &material);
private:
	
	void processMesh(gltf::data &data, gltf::mesh const &mesh, Vec<SharedPtr<Buffer>> &views);
	void processMeshAndSkin(gltf::data &data, gltf::mesh &mesh, gltf::skin &skin);;

	_NODISCARD static void processPrimitiveAttribsIntoVertexVector(
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
	Vec<std::future<void>> async_tasks_;
	
	Vec<SharedPtr<Material>> materials_;
	
public:
	
	enum class MeshLoaderType {
		eStandard,
		eMeshShader
	};
	
	struct Prim {
		MeshLoaderType loader_type;
		RID bind_group;
		RID vertex_buffer;
		RID meshlet_vertices_buffer;
		RID meshlet_triangles_buffer;
		RID meshlets_buffer;
		u64 vertex_offset;
		u64 index_count;
		SharedPtr<Material> material;
		u32 meshlet_count;
	};
	
#ifdef _DEBUG
public:
#else
private:
#endif
	Vec<Meshlet> meshlets;
	
	//VkDeviceSize vertex_offset;
	VkDeviceSize vertex_buffer_size_;
	//VkDeviceSize index_count_;
	//RID buffer_;
	std::vector<Prim> buffers_;
};