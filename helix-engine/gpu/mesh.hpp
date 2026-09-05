// ReSharper disable CppClangTidyClangDiagnosticPadded
#pragma once

// mesh and attributes n such

#include "driver.hpp"
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
	struct Primitive;
	struct Mesh;
	struct Material;
	struct Data;
	class Accessor;
}

struct PrimAttribResult;

#pragma pack(push, 1)
struct SkinnedVertex {
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

	static VertexInputDescriptor input_state() {
		static Vector input_attribute_descriptor = {
			VertexInputAttributeDescriptor{
				.location = 0,
				.binding = 0,
				.format = gfx::Format::eRgb32Sfloat,
				.offset = offsetof(Vertex, position)
			},
			VertexInputAttributeDescriptor{
				.location = 1,
				.binding = 0,
				.format = gfx::Format::eRgb32Sfloat,
				.offset = offsetof(Vertex, normal)
			},
			VertexInputAttributeDescriptor{
				.location = 2,
				.binding = 0,
				.format = gfx::Format::eRgba32Sfloat,
				.offset = offsetof(Vertex, tangent)
			},
			VertexInputAttributeDescriptor{
				.location = 3,
				.binding = 0,
				.format = gfx::Format::eRg32Sfloat,
				.offset = offsetof(Vertex, texcoord0)
			}
		};
		
		VertexInputBindingDescriptor binding_descriptor = {
			.binding = 0,
			.stride = sizeof(Vertex),
			.input_rate = gfx::InputRate::eVertex
		};
		
		VertexInputDescriptor vertex_input_descriptor = {
			.bindings = { binding_descriptor },
			.attributes = input_attribute_descriptor
		};
		
		return vertex_input_descriptor;
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

class Mesh : public IDisposable {
public:
	Mesh();
	Mesh(gltf::Data const &data); //< Loads all meshes under one umbrella.
	Mesh(gltf::Data const &data, _STD size_t mesh_id); //< loads a specific mesh.
	Mesh(gltf::Data &data, _STD size_t mesh_id, Vector<SharedPtr<gltf::Buffer>> &views); //< loads a specific mesh.
	Mesh(gltf::Data &data, _STD size_t mesh_id, _STD size_t skin_id); //< loads a specific mesh.
	~Mesh();

	Mesh(Mesh const &) = delete;
	Mesh& operator=(Mesh const &) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;
	
	_NODISCARD _STD size_t get_sub_mesh_count() const;
	void draw_sub_mesh(RenderPassInfo const &info, _STD size_t submesh);
	void draw_all_sub_meshes(RenderPassInfo const &info);
	void set_material(std::size_t index, SharedPtr<Material> const &material);
private:
	
	void process_mesh(gltf::Data &data, gltf::Mesh const &mesh, Vector<SharedPtr<gltf::Buffer>> &views);
	void process_mesh_and_skin(gltf::Data &data, gltf::Mesh &mesh, gltf::skin &skin);;

	_NODISCARD static void process_primitive_into_vertex_vector(
		gltf::Data &data,
		gltf::Primitive const &primitive,
		Vector<Vertex> &out_vertices
	);

	_NODISCARD GpuMesh process_primitive_into_separate_vector(
		gltf::Data &data,
		gltf::Primitive const &primitive,
		Vector<vec3> &position_vector,
		Vector<vec3> &normal_vector,
		Vector<vec4> &tangent_vector,
		Vector<vec2> &texcoord0_vector,
		Vector<vec2> &texcoord1_vector
	);

public:
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

private:
#ifdef _DEBUG
public:
#else
private:
#endif
	Vector<std::future<void>> async_tasks_;
	
	Vector<SharedPtr<Material>> materials_;
	
public:
	enum class MeshLoaderType {
		eStandard,
		eMeshShader
	};
	
	struct Primitive {
		MeshLoaderType loader_type;
		RID bind_group;
		RID vertex_buffer;
		RID index_buffer;
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
	Vector<Meshlet> meshlets;
	
	//VkDeviceSize vertex_offset;
	VkDeviceSize vertex_buffer_size_;
	//VkDeviceSize index_count_;
	//RID buffer_;
	std::vector<Primitive> buffers_;
};