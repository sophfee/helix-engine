// ReSharper disable CppCStyleCast
// ReSharper disable CppClangTidyCertErr33C

// ReSharper disable CppClangTidyMiscUseAnonymousNamespace
// ReSharper disable CppTooWideScope
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

#include "mesh.hpp"

#include <glm/gtx/string_cast.hpp>

#include "driver.hpp"
#include "png.h"
#include "backends/vulkan_backend.hpp"
#include "engine/engine.h"
#include "engine/main-loop.hpp"
#include "renderers/renderer.hpp"

namespace {
	template <typename T>
	[[maybe_unused]] T const *accessorPtr(gltf::data const &data, gltf::id const accessor_id) {
		gltf::accessor const &acc = data.accessors[accessor_id];
		assert(
			acc.stride() == sizeof(T) &&
			"Stride (size of each contiguous element) of the given GLTFAccessor is unequal to the type parameter T.");
		gltf::buffer_view const &bv = data.buffer_views[acc.bufferView()];
		assert(
			bv.stride == acc.stride() &&
			"Accessor stride (procedural) and Buffer view stride (set by GLTF file) are inequal.");
		gltf::buffer const &buf = data.buffers[bv.buffer];
		return reinterpret_cast<T const*>(buf.data().data() + bv.offset + acc.offset());
	}

	template <typename T>
	std::span<T> accessorSpan(gltf::data const &data, gltf::id const accessor_id) {
		gltf::accessor const &acc = data.accessors[accessor_id];
		const size_t acc_stride = acc.stride();
		assert(
			acc_stride == sizeof(T) &&
			"Stride (size of each contiguous element) of the given GLTFAccessor is unequal to the type parameter T.");
		gltf::buffer_view const &bv = data.buffer_views[acc.bufferView()];
		assert(
			(bv.stride == acc.stride() || bv.stride == 0) &&
			"Accessor stride (procedural) and Buffer view stride (set by GLTF file) are inequal.");
		gltf::buffer const &buf = data.buffers[bv.buffer];
		T *pointer = reinterpret_cast<T*>(const_cast<char*>(buf.data().data() + bv.offset + acc.offset()));
		std::span spaniard(pointer, acc.count());
		return spaniard;
	}
}

#define GLTF_USE_MANY_BUFFERS

Mesh::Mesh() {
}

Mesh::Mesh(gltf::data const &data) {
}

Mesh::Mesh(gltf::data const &data, _STD size_t const mesh_id) {
}

Mesh::Mesh(gltf::data &data, std::size_t const mesh_id, Vector<SharedPtr<Buffer>> &views) {
	process_mesh(data, data.meshes[mesh_id], views);
}

Mesh::Mesh(gltf::data &data, _STD size_t const mesh_id, [[maybe_unused]] _STD size_t skin_id) {
}

Mesh::~Mesh() {
	GraphicsBackend *driver = GraphicsDriver::get();
	for (const Primitive &prim : buffers_) {
		driver->destroy_buffer(prim.vertex_buffer);
		driver->destroy_buffer(prim.meshlet_vertices_buffer);
		driver->destroy_buffer(prim.meshlet_triangles_buffer);
		driver->destroy_buffer(prim.meshlets_buffer);
	}
}

_STD size_t Mesh::get_sub_mesh_count() const {
	return buffers_.size();
}

void Mesh::draw_sub_mesh(RenderPassInfo const &info, _STD size_t const submesh) {
}

void Mesh::draw_all_sub_meshes(RenderPassInfo const &info) {
	//if (subMeshCount() <= 0) assert(false);

	const RID cmd = info.cmd;

	//size_t vertex_offset = 0llu;
	//size_t index_offset = 0llu;

	GraphicsBackend *driver = GraphicsDriver::get();
	for (const Primitive &prim : buffers_) {
		
		SharedPtr<Material> const &material = prim.material;
		
		//if (!material)
		//	continue;
		if (material->bind_group_.lower == 0)
			continue;
		
		PushConstantRangeDescriptor descriptor{
			.visibility = gfx::ShaderStage::eVertex | gfx::ShaderStage::eFragment,
			.offset = sizeof(float4x4) + sizeof(GpuDeviceAddress) * 2,
			.size = sizeof(GpuDeviceAddress) * 4 + sizeof(u32)
		};
		
		driver->bind_vertex_buffer(cmd, VertexBufferDescriptor{
			.buffer = prim.vertex_buffer,
			.binding = 0,
			.offset = 0
		});
		
		driver->bind_index_buffer(cmd, IndexBufferDescriptor{
			.buffer = prim.vertex_buffer,
			.index_type = gfx::IndexType::eUInt32,
			.offset = prim.vertex_offset,
		});
		
		driver->set_bind_group(cmd, info.pipeline_layout, 0, material->bind_group_, gfx::ShaderStage::eFragment);
		driver->draw_indexed(cmd, static_cast<u32>(prim.index_count), 1, 0, 0, 0);
		
		//constexpr uint32_t taskDispatchX = 32;
		//uint32_t xCount = (meshlet_count + (taskDispatchX - 1)) / taskDispatchX;
		//driver->DispatchMesh(cmd, meshlet_count, 1, 1);
	}
}

void Mesh::set_material(std::size_t const index, SharedPtr<Material> const &material) {
}


constexpr auto alloc_block_step = 0x100000;

#undef min
#undef max


void Mesh::process_mesh_and_skin(gltf::data &data, gltf::mesh &mesh, gltf::skin &skin) {
}

static void loadDDS(gltf::image const &image, std::shared_ptr<RID> const &impl) {
	FILE *F;
	errno_t const ore = fopen_s(&F, image.file.c_str(), "rb");
	assert(ore == 0 && F != nullptr);
	std::string err;
	Error const res = DDS_UploadFromStdIO(F, impl, err);
	if (res != OK) __debugbreak();
	assert(res == OK);
	// The loader may close the file on its own.
	if (F)
		assert(fclose(F) == 0);
}

static void loadKTX2(gltf::image const &image, std::shared_ptr<Texture> const &impl) {
	auto const ktx2 = image.ktx2_texture;
	// Error const res = ktx::textureLoad(ktx2, impl->texture_object_);
	//assert(res == OK);

	ktxTexture_Destroy(ktx2);
}

static void loadPNGAsync_Inner(int h, void *output, gltf::image const &image, std::shared_ptr<Texture> const &impl) {
}


static RID loadPNGAsync(Mesh &mesh, gltf::image const &image, std::shared_ptr<RID> impl) {
	GraphicsBackend *driver = GraphicsDriver::get();

	const ImageDescriptor desc{
		.label = "image",
		.format = gfx::Format::eRgba8Unorm,
		.usage = gfx::ImageUsage::eSampled | gfx::ImageUsage::eTransferDst,
		.size = uvec3(static_cast<u32>(4096), static_cast<u32>(4096), 1u)
	};
	RID real_rid = driver->create_image(desc);

	mesh.async_tasks_.push_back(std::async([&mesh, real_rid, image, impl] {
		GraphicsBackend *driver = GraphicsDriver::get();
		// std::shared_ptr should almost always be copied! The IDE will yell at you but this is good practice with concurrency.
		FILE *f;
		std::string uri(image.uri);
		assert(fopen_s(&f,image.uri.c_str(), "rb") == 0);

		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		png_infop info_ptr = png_create_info_struct(png_ptr);
		png_init_io(png_ptr, f);
		png_read_info(png_ptr, info_ptr);

		png_byte const bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		png_byte const channels = png_get_channels(png_ptr, info_ptr);
		int const w = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
		int const h = static_cast<int>(png_get_image_height(png_ptr, info_ptr));

		//driver->image_create(real_rid, desc);

		*impl = real_rid;

		png_byte const color_type = png_get_color_type(png_ptr, info_ptr);

		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(png_ptr);
		if (bit_depth == 16)
			png_set_strip_16(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);
		if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_PALETTE || color_type ==
		    PNG_COLOR_TYPE_GRAY)
			png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER); // adds opaque alpha
		
		
		png_read_update_info(png_ptr, info_ptr);

		size_t const rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		VkDeviceSize const alloc_size = rowbytes * h;

		const BufferDescriptor buffer_create_desc = {
			.size = static_cast<u64>(w * h * 4),
			.usage = gfx::BufferUsage::eTransferSrc,
			.memory_usage = gfx::MemoryUsage::eAuto,
			.allocation_hints = gfx::AllocationHint::eHostSequentialWrite | gfx::AllocationHint::eAllowTransferInstead |
			                    gfx::AllocationHint::eMapped
		};

		const RID staging_buffer = driver->create_buffer(buffer_create_desc);
		u8 *data = (u8*)driver->get_mapped_data(staging_buffer);

		std::vector<png_bytep> rowPointers(h);
		for (int i = 0; i < h; i++) {
			rowPointers[i] = (png_bytep)data + i * rowbytes;
		}

		png_read_image(png_ptr, rowPointers.data());
		for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(h); i++) {
			memcpy(data + i * rowbytes, rowPointers[i], rowbytes);
		}

		VkGraphicsBackend *vk = dynamic_cast<VkGraphicsBackend*>(driver);
		assert(
			vk != nullptr &&
			"load_image_from_buffer is currently only implemented on the Vulkan backend, and you are not using the Vulkan backend.");

		VkBufferImageCopy2 imageCopy2{
			.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
			.bufferRowLength = static_cast<u32>(w),
			.bufferImageHeight = static_cast<u32>(h),
			.imageSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageOffset = {0, 0, 0},
			.imageExtent = {static_cast<uint32_t>(w), static_cast<uint32_t>(h), 1}
		};
		const VkFence fence = vk->load_image_from_buffer(real_rid, staging_buffer, imageCopy2);

		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(f);

		vkWaitForFences(vk->get_device(), 1, &fence, VK_TRUE, UINT64_MAX);
		vk->destroy_buffer(staging_buffer); // lazy but i hope it works!
		vkDestroyFence(vk->get_device(), fence, nullptr);
	}));
	return real_rid;
}

#if 0
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
	}
	else if (image.external_data->data() != nullptr) {
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

		_STD wstring wImageUid = string_to_wstring(cached_image_path);

		FILE *compressed_data_file = fopen(cached_image_path.c_str(), "wb");
		assert(compressed_data_file != nullptr);

		u16 const size_data[2]{static_cast<u16>(image.size.x), static_cast<u16>(image.size.y)};
		assert(fwrite(size_data, sizeof(u16), 2, compressed_data_file) == 2);

		u8 const channels_image = static_cast<u8>(image.channels);
		assert(fwrite(&channels_image, 1, 1, compressed_data_file) == 1);
		assert(
			fwrite(compressed_data.data(), 1, compressed_data.size(), compressed_data_file) == compressed_data.size());
		assert(fclose(compressed_data_file) == 0);
	}
}
#endif

static RID loadTexture(Mesh &mesh, gltf::data &data, gltf::texture &texture) {
	if (texture.impl_exists)
		//< Should this be marked as Likely? Texture loading is fairly lazy, in the sense we don't do any manual checking of existence up until now. Materials share textures quite often.
		return {};

	texture.impl = std::make_shared<RID>();
	texture.impl_exists = true;

	gltf::image const &image = data.images[texture.source];
	SharedPtr<RID> const impl = texture.impl;
	switch (image.image_type) {
	case gltf::eDDS:
		loadDDS(image, impl);
		break;
	case gltf::eKTX2:
		//loadKTX2(image, impl);
		break;
	case gltf::ePNG:
		return loadPNGAsync(mesh, image, impl);
	case gltf::eGeneric:
		break;
	}
	return *impl;
}

static RID loadTexture(Mesh &mesh, gltf::data &data, gltf::id const texture_id) {
	//< Bounds check.
	std::size_t const texture_index = static_cast<std::size_t>(texture_id);
	if (texture_index >= data.textures.size()) {
		assert(false && "Texture ID out of bounds");
		return {};
	}
	return loadTexture(mesh, data, data.textures[texture_index]);
}

static SharedPtr<Material> loadMaterial(Mesh &mesh, gltf::data &data, gltf::material &gltf_material) {
	if (gltf_material.impl != nullptr)
		return gltf_material.impl;

	const SharedPtr<Material> mtl = std::make_shared<Material>();

	mtl->roughness_ = gltf_material.pbr_metallic_roughness.roughness_factor;
	mtl->metallic_ = gltf_material.pbr_metallic_roughness.metallic_factor;
	mtl->emissive_color_mod_ = gltf_material.emissive_factor;

	if (gltf_material.pbr_metallic_roughness.base_color_texture.exists) {
		gltf::id const texture_id = gltf_material.pbr_metallic_roughness.base_color_texture.index;
		//std::future<RID> const impl = ;
		mtl->set_diffuse_texture(loadTexture(mesh, data, texture_id), gltf_material.pbr_metallic_roughness.base_color_factor);
		//mtl->setDiffuse(*impl, gltf_material.pbr_metallic_roughness.base_color_factor);
	}

	if (gltf_material.pbr_metallic_roughness.metallic_roughness_texture.exists) {
		gltf::id const texture_id = gltf_material.pbr_metallic_roughness.metallic_roughness_texture.index;
		mtl->set_orm_texture(loadTexture(mesh, data, texture_id));
	}

	if (gltf_material.normal_texture.exists) {
		gltf::id const texture_id = gltf_material.normal_texture.index;
		mtl->set_normal_texture(loadTexture(mesh, data, texture_id));
	}

	if (gltf_material.emissive_texture.exists) {
		gltf::id const texture_id = gltf_material.emissive_texture.index;
		mtl->set_emissive_texture(loadTexture(mesh, data, texture_id));
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

#if 0
static SharedPtr<Material> loadMaterial(Mesh &mesh, gltf::data &data, gltf::material &gltf_material) {
	if (gltf_material.impl != nullptr)
		return gltf_material.impl;

	auto const mtl = std::make_shared<Material>();

	mtl->roughness_ = gltf_material.pbr_metallic_roughness.roughness_factor;
	mtl->metallic_ = gltf_material.pbr_metallic_roughness.metallic_factor;
	mtl->emissive_color_mod_ = gltf_material.emissive_factor;

	if (gltf_material.pbr_metallic_roughness.base_color_texture.exists) {
		[[maybe_unused]] gltf::id const texture_id = gltf_material.pbr_metallic_roughness.base_color_texture.index;
		SharedPtr<RID> const impl = loadTexture(mesh, data, texture_id);
		// mtl->setDiffuse(impl, gltf_material.pbr_metallic_roughness.base_color_factor);
	}

	if (gltf_material.pbr_metallic_roughness.metallic_roughness_texture.exists) {
		[[maybe_unused]] gltf::id const texture_id = gltf_material.pbr_metallic_roughness.metallic_roughness_texture.
		                                                           index;
		SharedPtr<RID> const impl = loadTexture(mesh, data, texture_id);
		// mtl->orm_ = impl;
	}

	if (gltf_material.normal_texture.exists) {
		[[maybe_unused]] gltf::id const texture_id = gltf_material.normal_texture.index;
		SharedPtr<RID> const impl = loadTexture(mesh, data, texture_id);
		// mtl->normal_ = impl;
	}

	if (gltf_material.emissive_texture.exists) {
		[[maybe_unused]] gltf::id const texture_id = gltf_material.emissive_texture.index;
		SharedPtr<RID> const impl = loadTexture(mesh, data, texture_id);
		// mtl->emissive_ = impl;
	}

	gltf_material.impl = mtl;

	return mtl;
}

[[maybe_unused]] static SharedPtr<Material> loadMaterial(Mesh &mesh, gltf::data &data, u32 const material_id) {
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
[[maybe_unused]] static void iterate(gltf::data &data, Vector<Vertex> &out_vertices, gltf::id const acc,
                                     std::size_t const count) {
	Span<T> accessor_span = accessorSpan<T>(data, acc);
	for (std::size_t i = 0; i < count; ++i)
		*reinterpret_cast<T*>((u8*)(&out_vertices[i]) + OFFSET) = accessor_span[i];
}

template <typename T, std::size_t OFFSET>
[[maybe_unused]] static void iterate(gltf::data &data, Vector<Vertex> &out_vertices, gltf::id const acc,
                                     std::size_t const count, auto fun) {
	Span<T> accessor_span = accessorSpan<T>(data, acc);
	for (std::size_t i = 0; i < count; ++i) {
		*reinterpret_cast<T*>((u8*)(&out_vertices[i]) + OFFSET) = accessor_span[i];
		//*(T*)((u8*)(&out_vertices[i]) + OFFSET) = accessor_span[i];
		fun(*reinterpret_cast<T*>((u8*)(&out_vertices[i]) + OFFSET));
	}
}

void Mesh::process_primitive_into_vertex_vector(gltf::data &data, gltf::primitive const &primitive,
                                                   Vector<Vertex> &out_vertices) {
	_STD size_t count_ = 0;

	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		//vertex_size_ += gltf::componentsForType(accessor.type()) * gltf::sizeForComponentType(accessor.componentType());
		count_ = std::max(count_, accessor.count());
	}

	_STD size_t const start_index_ = out_vertices.size();
	out_vertices.resize(out_vertices.size() + count_);

	for (auto const &[name, accessor_id] : primitive.attributes) {
		switch (hash(name)) {
		case hash("POSITION"): {
			Span<vec3> positions = accessorSpan<vec3>(data, accessor_id);
			size_t i = 0;
			for (vec3 const &pos : positions) {
				if (i >= count_) break;
				out_vertices[i].position = pos;
				++i;
			}
			break;
		}
		case hash("NORMAL"): {
			Span<vec3> normals = accessorSpan<vec3>(data, accessor_id);
			size_t i = 0;
			for (vec3 const &normal : normals) {
				if (i >= count_) break;
				out_vertices[i].normal = normal;
				++i;
			}
			break;
		}
		case hash("TANGENT"): {
			Span<vec4> tangents = accessorSpan<vec4>(data, accessor_id);
			size_t i = 0;
			for (vec4 const &tangent : tangents) {
				if (i >= count_) break;
				out_vertices[i].tangent = tangent;
				++i;
			}
			break;
		}
		case hash("TEXCOORD_0"): {
			Span<vec2> uvs = accessorSpan<vec2>(data, accessor_id);
			size_t i = 0;
			for (vec2 const &uv : uvs) {
				if (i >= count_) break;
				out_vertices[i].texcoord0 = uv;
				++i;
			}
			break;
		}
		//case hash("TEXCOORD_1"): {
		//	Span<vec2> uvs = accessorSpan<vec2>(data, accessor_id);
		//	size_t i = 0;
		//	for (vec2 const& uv : uvs) {
		//		if (i >= count_) break;
		//		//out_vertices[i].texcoord1 = uv;
		//		++i;
		//	}
		//	break;
		//}
		default:
			break;
		}
	}
}

GpuMesh Mesh::process_primitive_into_separate_vector(gltf::data &data, gltf::primitive const &primitive,
                                                        Vector<vec3> &position_vector, Vector<vec3> &normal_vector,
                                                        Vector<vec4> &tangent_vector, Vector<vec2> &texcoord0_vector,
                                                        Vector<vec2> &texcoord1_vector) {
	_STD size_t count_ = 0;

	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		count_ = std::max(count_, accessor.count());
	}

	_STD size_t const start_index_ = position_vector.size();
	position_vector.reserve(position_vector.capacity() + count_);
	normal_vector.reserve(normal_vector.capacity() + count_);
	tangent_vector.reserve(tangent_vector.capacity() + count_);
	texcoord0_vector.reserve(texcoord0_vector.capacity() + count_);
	texcoord1_vector.reserve(texcoord1_vector.capacity() + count_);

	vec3 bounds_min(0.0f);
	vec3 bounds_max(0.0f);

	for (auto const &[name, accessor_id] : primitive.attributes) {
		switch (hash(name)) {
		case hash("POSITION"): {
			std::span<vec3> positions = accessorSpan<vec3>(data, accessor_id);
			position_vector.insert(position_vector.end(), positions.begin(), positions.end());
			for (vec3 const &pos : positions) {
				//< calculate min and max
				bounds_min.x = std::min(bounds_min.x, pos.x);
				bounds_min.y = std::min(bounds_min.y, pos.y);
				bounds_min.z = std::min(bounds_min.z, pos.z);
				bounds_max.x = std::max(bounds_max.x, pos.x);
				bounds_max.y = std::max(bounds_max.y, pos.y);
				bounds_max.z = std::max(bounds_max.z, pos.z);
				//position_vector.push_back(pos);
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

	// Ensure all attribute vectors have the same size (use defaults for missing attributes)
	if (position_vector.size() > normal_vector.size())
		normal_vector.resize(position_vector.size(), vec3(0.0f, 1.0f, 0.0f)); // Default normal is up
	if (position_vector.size() > tangent_vector.size())
		tangent_vector.resize(position_vector.size(), vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Default tangent
	if (position_vector.size() > texcoord0_vector.size())
		texcoord0_vector.resize(position_vector.size(), vec2(0.0f, 0.0f));
	if (position_vector.size() > texcoord1_vector.size())
		texcoord1_vector.resize(position_vector.size(), vec2(0.0f, 0.0f));

	GpuMesh mesh;
	mesh.localBoundsMin = bounds_min;
	mesh.localBoundsMax = bounds_max;
	mesh.instanceCount = 1;
	mesh.vertexCount = static_cast<u32>(count_);

	return mesh;
}

void Mesh::dispose() {
	
}

bool Mesh::disposed() const {
	return false;
}

static size_t optimize(Vector<Vertex> &vertices, Vector<u32> &indices, Vector<Vertex> &vertices_out, Vector<u32> &indices_out) {
	const std::size_t index_count = indices.size();
	const std::size_t vertex_count = vertices.size();
	std::vector<u32> remap(std::max(index_count, vertex_count));
	const size_t unique_vertex_count = meshopt_generateVertexRemap<u32>(remap.data(), indices.data(), index_count, vertices.data(), vertex_count, sizeof(Vertex));
	meshopt_remapIndexBuffer(indices_out.data(), indices.data(), index_count, remap.data());
	meshopt_remapVertexBuffer(vertices_out.data(), vertices.data(), vertex_count, sizeof(Vertex), remap.data());
	meshopt_optimizeVertexCache(indices_out.data(), indices_out.data(), index_count, vertex_count);
	meshopt_optimizeVertexFetch(vertices_out.data(), indices_out.data(), index_count, vertices_out.data(), vertex_count, sizeof(Vertex));
	return unique_vertex_count;
}

static void buildMeshlets(Vector<Vertex> const &vertices, Vector<u32> const &indices, Vector<Meshlet> &meshlets_out, Vector<u32> &meshlet_vertices_out, Vector<u8> &meshlet_triangles_out) {
	constexpr size_t max_vertices = 64;
	constexpr size_t max_triangles = 64;

	const size_t max_meshlets = meshopt_buildMeshletsBound(indices.size(), max_vertices, max_triangles);
	std::vector<meshopt_Meshlet> meshlets(max_meshlets);
	meshlet_vertices_out.resize(max_meshlets * max_vertices); // Each triangle
	meshlet_triangles_out.resize(max_meshlets * max_triangles * 3);

	const size_t meshlet_count = meshopt_buildMeshlets(
		meshlets.data(),
		meshlet_vertices_out.data(),
		meshlet_triangles_out.data(),
		indices.data(),
		indices.size(),
		reinterpret_cast<const float*>(vertices.data()),
		vertices.size(),
		sizeof(Vertex),
		max_vertices,
		max_triangles,
		0.0f
	);
	meshlets.resize(meshlet_count);
	meshlets_out.resize(meshlet_count);
	for (const meshopt_Meshlet &meshlet : meshlets) {
		meshopt_optimizeMeshlet(
			&meshlet_vertices_out[meshlet.vertex_offset],
			&meshlet_triangles_out[meshlet.triangle_offset],
			meshlet.triangle_count,
			meshlet.vertex_count
		);
	}
	
	constexpr u32 vertex_offset = 0;
	constexpr u32 meshlet_vertex_offset = 0;
	constexpr u32 meshlet_triangle_offset = 0;
	
	// Now shove into my little buffers
	for (std::size_t i = 0; i < meshlet_count; ++i) {
		const meshopt_Bounds bounds = meshopt_computeMeshletBounds(
			&meshlet_vertices_out[meshlets[i].vertex_offset],
			&meshlet_triangles_out[meshlets[i].triangle_offset],
			meshlets[i].triangle_count,
			reinterpret_cast<const float*>(vertices.data()),
			vertices.size(),
			sizeof(Vertex)
		);
		
		meshlets_out[i] = Meshlet{
			.bounding_sphere = float4(bounds.center[0], bounds.center[1], bounds.center[2], bounds.radius),
			.cone_apex = float3(bounds.cone_apex[0], bounds.cone_apex[1], bounds.cone_apex[2]),
			.cutoff = bounds.cone_cutoff,
			.cone_axis = float3(bounds.cone_axis[0], bounds.cone_axis[1], bounds.cone_axis[2]),
			.vertex_offset = vertex_offset,
			.meshlet_vertices_offset = meshlet_vertex_offset + meshlets[i].vertex_offset,
			.meshlet_triangle_offset = meshlet_triangle_offset + meshlets[i].triangle_offset,
			.meshlet_vertices_count = meshlets[i].vertex_count,
			.meshlet_triangle_count = meshlets[i].triangle_count
		};
	}
}

static void buildMeshPrimitiveForMeshShadingPipeline(const String& mesh_name, Mesh::Primitive& prim, Mesh* mesh, Vector<Vertex> &vertices, Vector<u32> &indices) {
	Vector<Meshlet> meshlets;
	Vector<u32> meshlet_vertices(indices.size());
	Vector<u8> meshlet_triangles(indices.size());
	buildMeshlets(vertices, indices, meshlets, meshlet_vertices, meshlet_triangles);
	
	prim.loader_type = Mesh::MeshLoaderType::eMeshShader;
	prim.vertex_buffer = gfx::allocate_buffer(mesh_name, vertices, gfx::BufferUsage::eShaderDeviceAddress);
	prim.meshlet_vertices_buffer = gfx::allocate_buffer(mesh_name, meshlet_vertices, gfx::BufferUsage::eShaderDeviceAddress);
	prim.meshlet_triangles_buffer = gfx::allocate_buffer(mesh_name, meshlet_triangles, gfx::BufferUsage::eShaderDeviceAddress);
	prim.meshlets_buffer = gfx::allocate_buffer(mesh_name, meshlets, gfx::BufferUsage::eShaderDeviceAddress);
	prim.meshlet_count = static_cast<u32>(meshlets.size());
}

static void buildMeshPrimitiveForStandardShadingPipeline(const String& mesh_name, Mesh::Primitive& prim, Mesh* mesh, Vector<Vertex> &vertices, Vector<u32> &indices) {
	prim.loader_type = Mesh::MeshLoaderType::eStandard;

	const BufferDescriptor descriptor{
		.label = mesh_name + " Vertex Buffer",
		.size = sizeof(Vertex) * vertices.size() + sizeof(u32) * indices.size(),
		.usage = gfx::BufferUsage::eVertex | gfx::BufferUsage::eIndex | gfx::BufferUsage::eShaderDeviceAddress,
		.memory_usage = gfx::MemoryUsage::eAuto,
		.allocation_hints = gfx::AllocationHint::eHostSequentialWrite | gfx::AllocationHint::eAllowTransferInstead |
		                    gfx::AllocationHint::eMapped
	};

	GraphicsBackend *driver = GraphicsDriver::get();
	prim.vertex_buffer = driver->create_buffer(descriptor);
	driver->set_buffer_name(prim.vertex_buffer, (mesh_name + " Vertex Buffer").c_str());

	u8 *mapped = (u8*)driver->get_mapped_data(prim.vertex_buffer);
	std::memcpy(mapped, vertices.data(), sizeof(Vertex) * vertices.size());
	std::memcpy(mapped + sizeof(Vertex) * vertices.size(), indices.data(), sizeof(u32) * indices.size());

	prim.index_count = indices.size();
	prim.vertex_offset = sizeof(Vertex) * vertices.size();
}

constexpr Mesh::MeshLoaderType loader = Mesh::MeshLoaderType::eStandard;

static Mesh::Primitive buildMeshPrimitive(const String& mesh_name, Mesh* mesh, Vector<Vertex> &vertices, Vector<u32> &indices) {
	Mesh::Primitive prim;
	switch (loader) {
	case Mesh::MeshLoaderType::eStandard:
		buildMeshPrimitiveForStandardShadingPipeline(mesh_name, prim, mesh, vertices, indices);
		break;
	case Mesh::MeshLoaderType::eMeshShader:
		buildMeshPrimitiveForMeshShadingPipeline(mesh_name, prim, mesh, vertices, indices);
		break;
	}

	return prim;
}

void Mesh::process_mesh(gltf::data &data, gltf::mesh const &mesh, Vector<SharedPtr<Buffer>> &views) {
	IRenderer *renderer = Main::get_renderer().value();
	assert(renderer && "Renderer should be initialized before processing meshes");

	GraphicsBackend *driver = GraphicsDriver::get();

	struct PrimRecord {
		SharedPtr<Material> material;
		u32 vertices_count;
		u32 indices_count;
	};

	std::vector<PrimRecord> records;
	records.reserve(mesh.primitives.size());

	char label_suffix = '0';

	Vector<GpuMaterial> gpu_materials;
	Vector<GpuMeshInstance> mesh_instances;

	u32 indices_count = 0u;

	for (gltf::primitive const &primitive : mesh.primitives) {
		switch (renderer->get_renderer_type()) {
		case RendererType::FORWARD: {
			SharedPtr<Material> material = loadMaterial(*this, data, primitive.material);

			Vector<Vertex> vertices;
			Vector<u32> indices;

			Vector<float3> positions;
			Vector<float3> normals;
			Vector<float4> tangents;
			Vector<float2> texcoord0s;
			Vector<float2> texcoord1s;

			[[maybe_unused]]
				GpuMesh gpu_mesh = process_primitive_into_separate_vector(
					data,
					primitive,
					positions,
					normals,
					tangents,
					texcoord0s,
					texcoord1s
				);

			if (primitive.indices != -1) {
				assert(data.accessors.size() > static_cast<std::size_t>(primitive.indices));
				gltf::accessor const &index_accessor = data.accessors[primitive.indices];
				switch (index_accessor.componentType()) {
				case gltf::component_type::eUnsignedByte: {
					Span<u8> indices_data = accessorSpan<u8>(data, primitive.indices);
					indices.reserve(indices_data.size());
					for (u8 index : indices_data) {
						indices.push_back(static_cast<u32>(index));
					}
					break;
				}
				case gltf::component_type::eUnsignedShort: {
					Span<u16> indices_data = accessorSpan<u16>(data, primitive.indices);
					indices.reserve(indices_data.size());
					for (u16 index : indices_data) {
						indices.push_back(static_cast<u32>(index));
					}
					break;
				}
				case gltf::component_type::eUnsignedInt: {
					Span<u32> indices_data = accessorSpan<u32>(data, primitive.indices);
					indices.reserve(indices_data.size());
					for (u32 index : indices_data) {
						indices.push_back(index);
					}
					break;
				}
				default:
					assert(false && "Unsupported index component type in glTF primitive");
					break;
				}
			}
			// Populate vertices from separate attribute vectors
			vertices.resize(positions.size());
			
			//	meshopt_generateTangents(
			//		(float*)tangents.data(),
			//		indices.data(),
			//		indices.size(),
			//		(float*)positions.data(),
			//		positions.size(),
			//		sizeof(vec3),
			//		(float*)normals.data(),
			//		sizeof(vec3),
			//		(float*)texcoord0s.data(),
			//		sizeof(vec2),
			//		0
			//	);
			
			indices_count = vertices.size();
			for (std::uint32_t i = 0; i < positions.size(); ++i) {
				vertices[i].position = positions[i];
				vertices[i].normal = normals[i];
				vertices[i].tangent = tangents[i];
				vertices[i].texcoord0 = texcoord0s[i];
			}
			
			Vector<Vertex> optimized_vertices(vertices.size());
			Vector<u32> optimized_indices(indices.size());
			size_t unique_vertex_count = optimize(vertices, indices, optimized_vertices, optimized_indices);
			optimized_vertices.resize(unique_vertex_count);
			vertices = optimized_vertices;
			indices = optimized_indices;
			
			Primitive prim = buildMeshPrimitive(mesh.name, this, vertices, indices);
			prim.material = material;
			buffers_.push_back(prim);
			
			driver->wait_for_idle();
			
			++label_suffix;
			break;
		}
		default:
			assert(false && "Unsupported renderer type");
			break;
		}
	}
}
