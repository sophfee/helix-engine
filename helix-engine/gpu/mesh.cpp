// ReSharper disable CppCStyleCast
// ReSharper disable CppClangTidyCertErr33C

#include "mesh.hpp"
#include "gltf.h"
#include <Windows.h>

#include <future>
#include <cassert>
#include <utility>

#include "material.hpp"
#include "util.hpp"
#include "khr/ktx.h"
#include "khr/ktx_ext.h"
#include "loaders/dds.hpp"


// ─────────────────────────────────────────────────────────────────────────────
// *** CPP  —  add this include near the top of mesh.cpp
// ─────────────────────────────────────────────────────────────────────────────

#include "driver.hpp"
#include "engine/engine.h"
#include "engine/main-loop.hpp"
#include "engine/thread_pool.hpp"
#include "renderers/renderer.hpp"

template <typename T>
static T const *accessorPtr(gltf::data const &data, gltf::id const accessor_id) {
    gltf::accessor    const &acc = data.accessors[accessor_id];
    gltf::buffer_view const &bv  = data.buffer_views[acc.bufferView()];
    gltf::buffer      const &buf = data.buffers[bv.buffer];
    return reinterpret_cast<T const *>(buf.data().data() + bv.offset + acc.offset());
}

template <typename T>
static std::span<T> accessorSpan(gltf::data const &data, gltf::id const accessor_id) {
	gltf::accessor    const &acc = data.accessors[accessor_id];
	gltf::buffer_view const &bv  = data.buffer_views[acc.bufferView()];
	gltf::buffer      const &buf = data.buffers[bv.buffer];

	T *pointer = reinterpret_cast<T *>(const_cast<char *>(buf.data().data() + bv.offset + acc.offset()));
	std::span spaniard(pointer, acc.count());
	return spaniard;
}


#define GLTF_USE_MANY_BUFFERS

Mesh::Mesh() : is_skinned_(false) {}

Mesh::Mesh(gltf::data const &data) : is_skinned_(false) {
}

Mesh::Mesh(gltf::data const &data, _STD size_t const mesh_id) : is_skinned_(false), draw_command_offset(0),
                                                                draw_command_count_offset(0) {
}

Mesh::Mesh(gltf::data &data, std::size_t const mesh_id, Vec<SharedPtr<Buffer>> &views) : is_skinned_(false) {
	processMesh(data, data.meshes[mesh_id], views);
}

Mesh::Mesh(gltf::data &data, _STD size_t const mesh_id, [[maybe_unused]] _STD size_t skin_id) : is_skinned_(true){
	//processMesh(data, data.meshes[mesh_id]);
}

Mesh::~Mesh() {
	GraphicsDriver* driver = GraphicsDriver::singleton();
	driver->buffer_delete(buffer_);
}

_STD size_t Mesh::subMeshCount() const {
	return primitives_.size();
}

void Mesh::drawSubMesh(RenderPassInfo const &info, _STD size_t const submesh) {
	if (auto const &[material, __, _, aabb] = primitives_[submesh]; material) { //< Not really likely or unlikely I think.
		material->bind(info);
	}
}

void Mesh::drawAllSubMeshes(RenderPassInfo const &info) {
	if (mesh_count <= 0) assert(false);
	
	const RID pipeline = info.pipeline;
	const RID cmd = info.cmd;
	
	GraphicsDriver* driver = GraphicsDriver::singleton();
	driver->bind_vertex_buffer(cmd, buffer_);
	driver->bind_index_buffer(cmd, buffer_, vk::IndexType::eUint16, vertex_buffer_size_);
	driver->draw_indexed(cmd, 0, static_cast<u32>(index_count_));
	// __debugbreak();
}

void Mesh::setMaterial(std::size_t const index, SharedPtr<Material> const &material) {
	primitives_[index].material = material;
}


constexpr auto alloc_block_step = 0x100000;

#undef min
#undef max

AABB Mesh::processAABB(Vec<Vertex> const &vertices) {
	vec3 minAABB(std::numeric_limits<float>::max());
	vec3 maxAABB(std::numeric_limits<float>::min());
	for (Vertex const &vertex : vertices) {
		minAABB.x = std::min(minAABB.x, vertex.position.x);
		minAABB.y = std::min(minAABB.y, vertex.position.y);
		minAABB.z = std::min(minAABB.z, vertex.position.z);
		maxAABB.x = std::max(maxAABB.x, vertex.position.x);
		maxAABB.y = std::max(maxAABB.y, vertex.position.y);
		maxAABB.z = std::max(maxAABB.z, vertex.position.z);
	}
	return { minAABB, maxAABB };
}

void Mesh::processMeshAndSkin(gltf::data &data, gltf::mesh &mesh, gltf::skin &skin) {
}
#if 0 // do this later

static void loadDDS(gltf::image const &image, std::shared_ptr<Texture> const &impl) {
	FILE *F;
	errno_t const ore = fopen_s(&F, image.file.c_str(), "rb");
	assert(ore == 0 && F != nullptr);
	std::string err;
	Error const res = DDS_UploadFromStdIO(F, impl, err);
	if (res != OK) __debugbreak();
	assert(res == OK);
	// The loader may close the file on its own.
	if (F) assert(fclose(F) == 0);
}

static void loadKTX2(gltf::image const &image, std::shared_ptr<Texture> const &impl) {
	auto const ktx2 = image.ktx2_texture;
	// Error const res = ktx::textureLoad(ktx2, impl->texture_object_);
	//assert(res == OK);
	
	ktxTexture_Destroy(ktx2);
}

static void loadPNGAsync_Inner(int h, void *output, gltf::image const &image, std::shared_ptr<Texture> const &impl) {
}


static std::future<void> loadPNGAsync(Mesh &mesh, gltf::image const &image, std::shared_ptr<RID> impl) {
	return ThreadPool::singleton()->addTaskToQueue([&mesh, &image, impl] { // std::shared_ptr should almost always be copied! The IDE will yell at you but this is good practice with concurrency.
		using namespace gl;
		FILE *f;
		std::string uri(image.uri);
		assert(fopen_s(&f,image.uri.c_str(), "rb") == 0);

		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, my_png_err, my_png_warn);
		png_infop info_ptr = png_create_info_struct(png_ptr);
		png_init_io(png_ptr, f);
		png_read_info(png_ptr, info_ptr);

		png_byte const bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		png_byte const channels = png_get_channels(png_ptr, info_ptr);
		int const w = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
		int const h = static_cast<int>(png_get_image_height(png_ptr, info_ptr));
		size_t const rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		VkDeviceSize const alloc_size = rowbytes * h;

		const VkBufferCreateInfo bufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = alloc_size,
			.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
		};

		constexpr VmaAllocationCreateInfo allocationCreateInfo = {
			.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT 
				| VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT 
				| VMA_ALLOCATION_CREATE_MAPPED_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO
		};

		GraphicsDriver* driver = GraphicsDriver::singleton();
		const RID staging_buffer = driver->buffer_create(bufferCreateInfo, allocationCreateInfo);
		void* data = driver->buffer_get_mapped_address(staging_buffer);
		
		std::vector<png_bytep> rowPointers(h);
		for (int i = 0; i < h; i++) {
			rowPointers[i] = (png_bytep)data + i * rowbytes;
		}
		png_read_image(png_ptr, rowPointers.data());
		for (int i = 0; i < h; i++) {
			memcpy(data + i * rowbytes, rowPointers[i], rowbytes);
		}
		
		driver->image_load_from_buffer()
		
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(f);

		std::cout << "Finished loading PNG " <<  uri << " asynchronously.\n";
	});
}

static void loadPNG(gltf::image const &image, std::shared_ptr<Texture> const &impl) {
	std::string const cached_image_path = ".local/img-cache/" + std::to_string(image.hash_value) + ".hltx";
	
	bool const image_is_compressed = image.compressed || image.is_dds || image.is_ktx2;
	gl::InternalFormat internal_format;
	gl::PixelFormat pixel_format;
	channelsToInternalFormat(image.channels, image_is_compressed, internal_format, pixel_format);
	impl->allocate(image.size, 1, internal_format);
	if (image.compressed) {
		impl->uploadImage2D(
			image.external_data->data(),
			0,
			ivec2(0, 0),
			image.size,
			pixel_format,
			gl::PixelType::UnsignedByte
		);
		impl->setFilter(gl::TextureMinFilter::LinearMipmapLinear, gl::TextureMagFilter::Linear);
	} else if (image.external_data->data() != nullptr) {
		impl->uploadImage2D(
			image.external_data->data(),
			0,
			ivec2(0, 0),
			image.size,
			pixel_format,
			gl::PixelType::UnsignedByte
		);
			
		image.external_data->clear();
		image.external_data->shrink_to_fit();

		glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
			
		std::vector<u8> compressed_data;
		impl->imageData(compressed_data, 0);

		_STD wstring wImageUid = stringToWideString(cached_image_path);
			
		FILE *compressed_data_file = fopen(cached_image_path.c_str(), "wb");
		assert(compressed_data_file != nullptr);

		u16 const size_data[2] { static_cast<u16>(image.size.x), static_cast<u16>(image.size.y) };
		assert(fwrite(size_data, sizeof(u16), 2, compressed_data_file) == 2);

		u8 const channels_image = static_cast<u8>(image.channels); 
		assert(fwrite(&channels_image, 1, 1, compressed_data_file) == 1);
		assert(fwrite(compressed_data.data(), 1, compressed_data.size(), compressed_data_file) == compressed_data.size());
		assert(fclose(compressed_data_file) == 0);
	}
}

static SharedPtr<Texture> loadTexture(Mesh &mesh, gltf::data &data, gltf::texture &texture) {
	if (texture.impl != nullptr) //< Should this be marked as Likely? Texture loading is fairly lazy, in the sense we don't do any manual checking of existence up until now. Materials share textures quite often.
		return texture.impl;
	
	texture.impl = std::make_shared<Texture>(gl::TextureTarget::Texture2D);
	
	auto &[mag_filter, min_filter, wrap_s_mode, wrap_t_mode] = data.samplers[texture.sampler];
	gltf::image const &image = data.images[texture.source];

	SharedPtr<Texture> const impl = texture.impl;
	switch (image.image_type) {
		case gltf::eDDS:
			loadDDS(image, impl);
			break;
		case gltf::eKTX2:
			loadKTX2(image, impl);
			break;
		case gltf::ePNG:
			mesh.async_tasks_.push_back(loadPNGAsync(mesh, image, impl));
			break;
		case gltf::eGeneric:
			break;
	}

	impl->setFilter(min_filter, mag_filter);
	impl->setWrapMode(gl::TextureWrapMode::ClampToEdge, wrap_s_mode, wrap_t_mode);
	impl->enableAnisotropicFiltering();
	return impl;
}

static SharedPtr<Texture> loadTexture(Mesh &mesh, gltf::data &data, gltf::id const texture_id) {
	//< Bounds check.
	std::size_t const texture_index = static_cast<std::size_t>(texture_id);
	if (texture_index >= data.textures.size()) {
		assert(false && "Texture ID out of bounds");
		return nullptr;
	}
	return loadTexture(mesh, data, data.textures[texture_index]);
}

static SharedPtr<Material> loadMaterial(Mesh &mesh, gltf::data &data, gltf::material &gltf_material) {
	if (gltf_material.impl != nullptr)
		return gltf_material.impl;

	auto const mtl = std::make_shared<Material>();
	
	mtl->roughness_ = gltf_material.pbr_metallic_roughness.roughness_factor;
	mtl->metallic_  = gltf_material.pbr_metallic_roughness.metallic_factor;
	mtl->emissive_color_mod_ = gltf_material.emissive_factor;
	
	if (gltf_material.pbr_metallic_roughness.base_color_texture.exists) {
		gltf::id const texture_id = gltf_material.pbr_metallic_roughness.base_color_texture.index;
		SharedPtr<Texture> const impl = nullptr; // loadTexture(mesh, data, texture_id);
		mtl->setDiffuse(impl, gltf_material.pbr_metallic_roughness.base_color_factor);
	}
	
	if (gltf_material.pbr_metallic_roughness.metallic_roughness_texture.exists) {
		gltf::id const texture_id = gltf_material.pbr_metallic_roughness.metallic_roughness_texture.index;
		SharedPtr<Texture> const impl = nullptr; // loadTexture(mesh, data, texture_id);
		mtl->orm_ = impl;
	}
	
	if (gltf_material.normal_texture.exists) {
		gltf::id const texture_id = gltf_material.normal_texture.index;
		SharedPtr<Texture> const impl = nullptr; // loadTexture(mesh, data, texture_id);
		mtl->normal_ = impl;
	}

	if (gltf_material.emissive_texture.exists) {
		gltf::id const texture_id = gltf_material.emissive_texture.index;
		SharedPtr<Texture> const impl = nullptr; // loadTexture(mesh, data, texture_id);
		mtl->emissive_ = impl;
	}

	gltf_material.impl = mtl;

	return mtl;
}

static SharedPtr<Material> loadMaterial(Mesh &mesh, gltf::data &data, u32 const material_id) {
	std::size_t const material_index = static_cast<std::size_t>(material_id);
	if (material_index >= data.materials.size()) {
		assert(false && "Material ID out of bounds");
		return nullptr;
	}

	gltf::material &gltf_material = data.materials[material_index];
	return loadMaterial(mesh, data, gltf_material);
}
#else

static SharedPtr<Material> loadMaterial(Mesh &mesh, gltf::data &data, gltf::material &gltf_material) {
	if (gltf_material.impl != nullptr)
		return gltf_material.impl;

	auto const mtl = std::make_shared<Material>();
	
	mtl->roughness_ = gltf_material.pbr_metallic_roughness.roughness_factor;
	mtl->metallic_  = gltf_material.pbr_metallic_roughness.metallic_factor;
	mtl->emissive_color_mod_ = gltf_material.emissive_factor;
	
	if (gltf_material.pbr_metallic_roughness.base_color_texture.exists) {
		gltf::id const texture_id = gltf_material.pbr_metallic_roughness.base_color_texture.index;
		SharedPtr<Texture> const impl = nullptr; // loadTexture(mesh, data, texture_id);
		// mtl->setDiffuse(impl, gltf_material.pbr_metallic_roughness.base_color_factor);
	}
	
	if (gltf_material.pbr_metallic_roughness.metallic_roughness_texture.exists) {
		gltf::id const texture_id = gltf_material.pbr_metallic_roughness.metallic_roughness_texture.index;
		SharedPtr<Texture> const impl = nullptr; // loadTexture(mesh, data, texture_id);
		// mtl->orm_ = impl;
	}
	
	if (gltf_material.normal_texture.exists) {
		gltf::id const texture_id = gltf_material.normal_texture.index;
		SharedPtr<Texture> const impl = nullptr; // loadTexture(mesh, data, texture_id);
		// mtl->normal_ = impl;
	}

	if (gltf_material.emissive_texture.exists) {
		gltf::id const texture_id = gltf_material.emissive_texture.index;
		SharedPtr<Texture> const impl = nullptr; // loadTexture(mesh, data, texture_id);
		// mtl->emissive_ = impl;
	}

	gltf_material.impl = mtl;

	return mtl;
}

static SharedPtr<Material> loadMaterial(Mesh &mesh, gltf::data &data, u32 const material_id) {
	std::size_t const material_index = static_cast<std::size_t>(material_id);
	if (material_index >= data.materials.size()) {
		assert(false && "Material ID out of bounds");
		return nullptr;
	}

	gltf::material &gltf_material = data.materials[material_index];
	return loadMaterial(mesh, data, gltf_material);
}

#endif

#ifdef GLTF_USE_MANY_BUFFERS
#define SetupAttribute(N0,N1,C,D,E,F) applyAccessorAsAttribute(C,D,E,F, views)
#else
#define SetupAttribute(A,B,D,E,F,G,H,I) applyAccessorAsAttributeSingleBufferUnskinned(A,B,D,E,F,G,H,I)
#endif

template <typename T, std::size_t OFFSET>
static void iterate(gltf::data &data, Vec<Vertex> &out_vertices, gltf::id const acc, std::size_t const count) {
	const T* accessor_span = accessorPtr<T>(data, acc);
	for (std::size_t i = 0; i < count; ++i)
		*(T*)((u8*)(&out_vertices[i]) + OFFSET) = accessor_span[i];
}

template <typename T, std::size_t OFFSET>
static void iterate(gltf::data &data, Vec<Vertex> &out_vertices, gltf::id const acc, std::size_t const count, auto fun) {
	const T* accessor_span = accessorPtr<T>(data, acc);
	for (std::size_t i = 0; i < count; ++i) {
		*(T*)((u8*)(&out_vertices[i]) + OFFSET) = accessor_span[i];
		fun(*(T*)((u8*)(&out_vertices[i]) + OFFSET));
	}
}

AABB Mesh::processPrimitiveAttribsIntoVertexVector(gltf::data &data, gltf::primitive const &primitive, Vec<Vertex> &out_vertices) {
	_STD size_t count_ = 0;

	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		//vertex_size_ += gltf::componentsForType(accessor.type()) * gltf::sizeForComponentType(accessor.componentType());
		count_ = std::max(count_, accessor.count());
	}
	
	_STD size_t const start_index_ = out_vertices.size();
	out_vertices.resize(out_vertices.size() + count_);
	
	vec3 bounds_min(0.0f);
	vec3 bounds_max(0.0f);
	
	for (auto const &[name, accessor_id] : primitive.attributes) {
		u8 const *accessor_data = accessorPtr<u8>(data, accessor_id);
		switch (hash(name)) {
			case hash("POSITION"): {
				iterate<vec3, offsetof(Vertex, position)>(data, out_vertices, accessor_id, count_, [&](vec3 const &pos) {
					bounds_min.x = std::min(bounds_min.x, pos.x);
					bounds_min.y = std::min(bounds_min.y, pos.y);
					bounds_min.z = std::min(bounds_min.z, pos.z);
					bounds_max.x = std::max(bounds_max.x, pos.x);
					bounds_max.y = std::max(bounds_max.y, pos.y);
					bounds_max.z = std::max(bounds_max.z, pos.z);
				});
				break;
			}
			case hash("NORMAL"):
				iterate<vec3, offsetof(Vertex, normal)>(data, out_vertices, accessor_id, count_);
				break;
			case hash("TANGENT"):
				iterate<vec4, offsetof(Vertex, tangent)>(data, out_vertices, accessor_id, count_);
				break;
			case hash("TEXCOORD_0"):
				iterate<vec2, offsetof(Vertex, texcoord0)>(data, out_vertices, accessor_id, count_);
				break;
			case hash("TEXCOORD_1"):
				iterate<vec2, offsetof(Vertex, texcoord1)>(data, out_vertices, accessor_id, count_);
				break;
			default:
				break;
		}
	}
	
	return AABB(bounds_min, bounds_max);
}

GpuMesh Mesh::processPrimitiveAttribsIntoSeparateVector(gltf::data &data, gltf::primitive const &primitive, Vec<vec3> &position_vector, Vec<vec3> &normal_vector, Vec<vec4> &tangent_vector, Vec<vec2> &texcoord0_vector, Vec<vec2> &texcoord1_vector) {
	_STD size_t count_ = 0;

	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		//vertex_size_ += gltf::componentsForType(accessor.type()) * gltf::sizeForComponentType(accessor.componentType());
		count_ = std::max(count_, accessor.count());
	}
	
	_STD size_t const start_index_ = position_vector.size();
	position_vector.resize(position_vector.size() + count_);
	normal_vector.resize(normal_vector.size() + count_);
	tangent_vector.resize(tangent_vector.size() + count_);
	texcoord0_vector.resize(texcoord0_vector.size() + count_);
	texcoord1_vector.resize(texcoord1_vector.size() + count_);
	
	vec3 bounds_min(0.0f);
	vec3 bounds_max(0.0f);
	
	for (auto const &[name, accessor_id] : primitive.attributes) {
		switch (hash(name)) {
			case hash("POSITION"): {
				std::span<vec3> positions = accessorSpan<vec3>(data, accessor_id);
				//position_vector.insert(position_vector.end(), positions.begin(), positions.end());
				for (vec3 const &pos : positions) { //< calculate min and max
					bounds_min.x = std::min(bounds_min.x, pos.x);
					bounds_min.y = std::min(bounds_min.y, pos.y);
					bounds_min.z = std::min(bounds_min.z, pos.z);
					bounds_max.x = std::max(bounds_max.x, pos.x);
					bounds_max.y = std::max(bounds_max.y, pos.y);
					bounds_max.z = std::max(bounds_max.z, pos.z);
					position_vector.push_back(pos);
				}
				break;
			}
			case hash("NORMAL"): {
				std::span<vec3> normals = accessorSpan<vec3>(data, accessor_id);
				normal_vector.insert(normal_vector.end(), normals.begin(), normals.end());
				break;
			}
			case hash("TANGENT"): {
				std::span<vec4> tangents = accessorSpan<vec4>(data, accessor_id);
				tangent_vector.insert(tangent_vector.end(), tangents.begin(), tangents.end());
				break;
			}
			case hash("TEXCOORD_0"): {
				std::span<vec2> texcoords = accessorSpan<vec2>(data, accessor_id);
				texcoord0_vector.insert(texcoord0_vector.end(), texcoords.begin(), texcoords.end());
				break;
			}
			case hash("TEXCOORD_1"): {
				std::span<vec2> texcoords1 = accessorSpan<vec2>(data, accessor_id);
				texcoord1_vector.insert(texcoord1_vector.end(), texcoords1.begin(), texcoords1.end());
				break;
			}
			default:
				break;
		}
	}

	GpuMesh mesh;
	mesh.localBoundsMin = bounds_min;
	mesh.localBoundsMax = bounds_max;
	mesh.instanceCount = 1;
	mesh.vertexCount = static_cast<u32>(count_);
	
	return mesh;
}

void Mesh::processMesh(gltf::data &data, gltf::mesh const &mesh, Vec<SharedPtr<Buffer>> &views) {
	IRenderer *renderer = Main::renderer().value();
	assert(renderer && "Renderer should be initialized before processing meshes");

	struct PrimRecord {
		SharedPtr<Material>				material;
		u32								vertices_count;
		u32								indices_count;
		AABB							aabb;
	};

	std::vector<PrimRecord> records;
	records.reserve(mesh.primitives.size());

	char label_suffix = '0';

	Vec<GpuMaterial> gpu_materials;
	Vec<GpuMeshInstance> mesh_instances;
	
	Vec<Vertex> vertices;
	
	Vec<vec3> positions;
	Vec<vec3> normals;
	Vec<vec4> tangents;
	Vec<vec2> texcoord0s;
	Vec<vec2> texcoord1s;
	Vec<u16>  indices;
	
	for (gltf::primitive const &primitive : mesh.primitives) {
		switch (renderer->rendererType()) {
			case RendererType::DEFERRED: {
				/*
				auto vertex_array = std::make_shared<VertexArray>();
				vertex_array->bind();
				vertex_array->setLabel(mesh.name + "#" + label_suffix);
				
				auto [aabb, tangent_future] = processPrimitiveAttribs(data, vertex_array, primitive, views);
				if (primitive.indices != -1) {
					assert(data.accessors.size() > static_cast<std::size_t>(primitive.indices));
					applyAccessorAsElementBuffer(data, vertex_array, data.accessors[primitive.indices], views, indices);
				}
				gpu_check;
				u32 const mat_id = primitive.material;
				records.push_back({
					.vertex_array   = std::move(vertex_array),
					.material       = static_cast<std::size_t>(mat_id) < data.materials.size()
										  ? loadMaterial(*this, data, mat_id) : nullptr,
					.vertices = {}, //< Don't preserve the pure vertices or indices in deferred mode
					.indices = {},
					.aabb        = aabb
				});

				++label_suffix;
				*/
				break;
			}
			case RendererType::FORWARD: {
				std::size_t const start_vertices = positions.size();
				std::size_t start_indices = indices.size();
				// GpuMesh gpu_mesh = processPrimitiveAttribsIntoSeparateVector(data, primitive, positions, normals, tangents, texcoord0s, texcoord1s);
				Vec<Vertex> primitive_vertices;
				AABB discard = processPrimitiveAttribsIntoVertexVector(data, primitive, primitive_vertices);
				
				for (Vertex vertex : primitive_vertices) {
					vertices.push_back(vertex);
				}
				
				std::size_t index_count;
				if (primitive.indices != -1) {
					assert(data.accessors.size() > static_cast<std::size_t>(primitive.indices));
					const u16* indicesData = accessorPtr<u16>(data, primitive.indices);
					for (u32 index = 0; index < data.accessor_count(primitive.indices); ++index) {
						indices.push_back(indicesData[index]);
					}
					index_count = data.accessor_count(primitive.indices);
				}

				u32 const mat_id = primitive.material;

				GpuMesh gpu_mesh;
				gpu_mesh.vertexCount  = static_cast<i32>(primitive_vertices.size());
				gpu_mesh.indexCount   = static_cast<u32>(index_count);
				gpu_mesh.vertexOffset = static_cast<i32>(start_vertices);
				gpu_mesh.indexOffset  = static_cast<u32>(start_indices);

				SharedPtr<Material> used_material = data.materials[static_cast<std::size_t>(mat_id)].impl;
				if (used_material == nullptr) {
					used_material = loadMaterial(*this, data, mat_id);
				}
				assert(used_material != nullptr && "Material should not be null after loading");

				u32 mesh_material_id = 0u;
				bool material_already_used_on_this_mesh = false;
				for (SharedPtr<Material> const &existing_material : materials_) {
					if (existing_material == used_material) {
						used_material = existing_material;
						material_already_used_on_this_mesh = true;
						break;
					}
					++mesh_material_id;
				}
				if (!material_already_used_on_this_mesh) {
					gpu_materials.push_back(used_material->gpu());
					materials_.push_back(used_material);
				}
				gpu_mesh.materialId = mesh_material_id;

				mesh_instances.push_back({
					static_cast<u32>(meshes.size()),
					0
				});
				meshes.push_back(gpu_mesh);

				++label_suffix;
				break;
			}
			default: 
				assert(false && "Unsupported renderer type");
				break;
		}
	}
	
	primitives_.reserve(primitives_.size() + records.size());
	
	
// #define VERTEX_LOOP_APPLY(ATTR) \
// 	for (std::uint32_t i = 0; i < ATTR##s.size(); ++i) \
// 		vertices[i].ATTR = ATTR##s[i]; \
// 	
// 	for (std::uint32_t i = 0; i < positions.size(); ++i) vertices[i].position = positions[i];
// 	for (std::uint32_t i = 0; i < normals.size(); ++i) vertices[i].normal = normals[i];
// 	for (std::uint32_t i = 0; i < tangents.size(); ++i) vertices[i].tangent = tangents[i];
// 	for (std::uint32_t i = 0; i < texcoord0s.size(); ++i) vertices[i].texcoord0 = texcoord0s[i];
// 	for (std::uint32_t i = 0; i < texcoord1s.size(); ++i) vertices[i].texcoord1 = texcoord1s[i];
// 	
// #undef VERTEX_LOOP_APPLY
	
	GraphicsDriver* driver = GraphicsDriver::singleton();
	
	VkBufferCreateInfo bufferCreateInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = vertices.size() * sizeof(Vertex) + indices.size() * sizeof(uint16_t),
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
	};
	
	VmaAllocationCreateInfo bufferAllocationCreateInfo{
		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
	};
	
	buffer_ = driver->buffer_create(bufferCreateInfo, bufferAllocationCreateInfo);
	u8* buffer_data = (u8*)driver->buffer_get_mapped_address(buffer_);
	
	memcpy(buffer_data, vertices.data(), vertices.size() * sizeof(Vertex));
	memcpy(buffer_data + vertices.size() * sizeof(Vertex), indices.data(), indices.size() * sizeof(uint16_t));
	
	vertex_buffer_size_ = vertices.size() * sizeof(Vertex);
	index_count_ = indices.size();
	
	mesh_count = static_cast<u32>(meshes.size());
	
	driver->buffer_set_allocation_name(buffer_, mesh.name.c_str());
	
	//printf("Finished loading mesh %s\n", mesh.name.c_str());
}