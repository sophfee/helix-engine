// ReSharper disable CppCStyleCast
// ReSharper disable CppClangTidyCertErr33C

#include <math.hpp>
#include "mesh.hpp"
#include "gltf.h"
#include <Windows.h>

#include <future>
#include <cassert>
#include <utility>

#include "material.hpp"
#include "texture.h"
#include "util.hpp"
#include "khr/ktx.h"
#include "khr/ktx_ext.h"
#include "loaders/dds.hpp"

#define GLTF_USE_MANY_BUFFERS

CSkin::CSkin() {
}

CSkin::~CSkin() {
}

Mesh::Mesh() : is_skinned_(false) {}

Mesh::Mesh(gltf::data const &data) : is_skinned_(false) {
	//for (gltf::mesh &mesh : data.meshes)
	//	processMesh(data, mesh);
	//primitives_.back().vertex_array->unbind();
	//processMaterials(data);
}

Mesh::Mesh(gltf::data const &data, _STD size_t const mesh_id) : is_skinned_(false){
	//processMesh(data, data.meshes[mesh_id]);
	//processMaterials(data);
}
Mesh::Mesh(gltf::data &data, std::size_t const mesh_id, Vec<SharedPtr<Buffer>> &views) : is_skinned_(false) {
	processMesh(data, data.meshes[mesh_id], views);
}

Mesh::Mesh(gltf::data &data, _STD size_t const mesh_id, [[maybe_unused]] _STD size_t skin_id) : is_skinned_(true){
	//processMesh(data, data.meshes[mesh_id]);
}

Mesh::~Mesh() {
}

_STD size_t Mesh::subMeshCount() const {
	return primitives_.size();
}

void Mesh::drawSubMesh(RenderPassInfo const &info, _STD size_t const submesh) const {
	auto const &[vertex_array,  material, aabb] = primitives_[submesh];
	material->bind(info);
	if (vertex_array) {
		vertex_array->bind();
		vertex_array->draw();
	}
}

void Mesh::drawAllSubMeshes(RenderPassInfo const &info) const {
	for (size_t i = 0; i < subMeshCount(); ++i)
		drawSubMesh(info, i);
}

bool Mesh::skinned() const {
	return skin_.has_value();
}
void Mesh::computeTangents(
	gltf::data &data,
	SharedPtr<VertexArray> const &vertex_array,
	gltf::primitive const &primitive,
	gltf::id const index_accessor,
	gltf::id const position_accessor,
		gltf::id const normal_accessor,
	gltf::id const texcoord_accessor) {

	/*
	if (compute_tangents == nullptr) {
		compute_tangents = std::make_unique<Program>("shaders\\compute_tangents.comp");
		
	}
	*/

	u32 const vertex_count = data.accessor_count(position_accessor);
	u32 const count = data.accessor_count(index_accessor);
	Vec<vec3> tan0(count);
	Vec<vec3> tan1(count);
	Vec<vec4> fin(count);

	for (size_t vID = 0; vID < vertex_count; ++vID) {
		for (u32 i = 0; i < count; i += 3) {
			u16 const *idx = data.make_cursor<u16>(index_accessor, static_cast<i32>(i));

			u16 const i0 = idx[0];
			u16 const i1 = idx[1];
			u16 const i2 = idx[2];
		
			vec3 const &pos0 = data.read_accessor<vec3>(position_accessor, i0);
			vec3 const &pos1 = data.read_accessor<vec3>(position_accessor, i1);
			vec3 const &pos2 = data.read_accessor<vec3>(position_accessor, i2);

			vec2 const &uv0 = data.read_accessor<vec2>(texcoord_accessor, i0);
			vec2 const &uv1 = data.read_accessor<vec2>(texcoord_accessor, i1);
			vec2 const &uv2 = data.read_accessor<vec2>(texcoord_accessor, i2);

			vec3 delta_pos1 = pos1 - pos0;
			vec3 delta_pos2 = pos2 - pos0;

			vec2 delta_uv1 = uv1 - uv0;
			vec2 delta_uv2 = uv2 - uv0;

			f32 const r = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);

			tan0[i0] += (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) * r;
			tan1[i0] += (delta_pos2 * delta_uv1.x - delta_pos1 * delta_uv2.x) * r;
		}
	}

	for (size_t i = 0; i < vertex_count; ++i) {
		vec3 const &normal = data.read_accessor<vec3>(normal_accessor, static_cast<i32>(i));
		vec3 const &tangent = tan0[i];
		vec3 const &binormal = tan1[i];

		// f32 normal_dot_tangent = glm::dot(normal, tangent);
		// vec3 normal_x_dot_product = normal * normal_dot_tangent;
		// Gram-Schmidt orthogonalization of tangent against normal
		f32 ndott = glm::dot(normal, tangent);
		vec3 tangent_vector = glm::normalize(tangent - normal * ndott);

		//  Handedness: sign of the triple product
		f32 handedness = glm::dot(glm::cross(normal, tangent), binormal) < 0.0f ? -1.0f : 1.0f;
		
		fin[i] = vec4(tangent_vector, handedness);
	}

	auto tangent_buffer = _STD make_shared<Buffer>();
	tangent_buffer->allocStorage(fin.size() * sizeof(vec4), fin.data(), gl::BufferStorageMask::DynamicStorageBit);
	tangent_buffer->upload(
		sizeof(vec4) * fin.size(), fin.data(),
		gl::BufferUsageARB::StaticDraw
	);
	buffers_.push_back(tangent_buffer);
	auto binding = vertex_array->vertex_buffer_count++;

#ifdef GLTF_USE_MANY_BUFFERS
	constexpr auto tangent_stride = sizeof(vec4);
#else
	constexpr auto tangent_stride = 64;
#endif

	vertex_array->setVertexBuffer(
		static_cast<u32>(binding),
		*tangent_buffer,
		tangent_stride,
		sizeof(vec4)
	);

	vertex_array->setAttribute({
		.index = 2,
		.binding = static_cast<i32>(binding),
		.size = 4,
		.stride = tangent_stride,
		.offset = 0,
		.type = EComponentType::SINGLE_FLOAT,
		.normalized = false
	});
}

constexpr auto alloc_block_step = 0x100000;

#undef min
#undef max

AABB Mesh::processAABB(Vec<StandardVertex> const &vertices) {
	vec3 minAABB(std::numeric_limits<float>::max());
	vec3 maxAABB(std::numeric_limits<float>::min());
	for (StandardVertex const &vertex : vertices) {
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
	//processMesh(data, mesh);
	auto ssbo_inv_bind_matrices = _STD make_shared<Buffer>();
	//ssbo_inv_bind_matrices->allocStorage(skin)
}

static void channelsToInternalFormat(int const channels, bool const compressed, gl::InternalFormat &internal_format, gl::PixelFormat &pixel_format) {
	switch (channels) {
		case 1:
			internal_format = gl::InternalFormat::R8;
			pixel_format = gl::PixelFormat::Red;
			break;
		case 2:
			internal_format = gl::InternalFormat::Rg8;
			pixel_format = gl::PixelFormat::Rg;
			break;
		case 3:
			if (compressed) {
				internal_format = gl::InternalFormat::CompressedRgbS3tcDxt1Ext;
				pixel_format = gl::PixelFormat::Rgb;
			}
			else {
				internal_format = gl::InternalFormat::Rgb8;
				pixel_format = gl::PixelFormat::Rgb;
			}
			break;
		case 4:
			if (compressed) {
				internal_format = gl::InternalFormat::CompressedRgbaS3tcDxt5Ext;
				pixel_format = gl::PixelFormat::Rgba;
			}
			else {
				internal_format = gl::InternalFormat::Rgba8;
				pixel_format = gl::PixelFormat::Rgba;
			}
			break;
		default:
			internal_format = gl::InternalFormat::Rgb8;
			pixel_format = gl::PixelFormat::Rgb;
			break;
	}
}

static void loadDDS(gltf::image const &image, std::shared_ptr<Texture> const &impl) {
	FILE *F;
	errno_t const ore = fopen_s(&F, image.file.c_str(), "rb");
	assert(ore == 0 && F != nullptr);
	std::string err;
	Error const res = DDS_UploadFromStdIO(F, impl->texture_object_, err);
	if (res != OK) __debugbreak();
	assert(res == OK);
	// The loader may close the file on it's own.
	if (F) assert(fclose(F) == 0);
}

static void loadKTX2(gltf::image const &image, std::shared_ptr<Texture> const &impl) {
	auto const ktx2 = image.ktx2_texture;
	Error const res = ktx::textureLoad(ktx2, impl->texture_object_);
	assert(res == OK);
	ktxTexture_Destroy(ktx2);
}

static void loadPNG(gltf::image const &image, std::shared_ptr<Texture> const &impl) {
	impl->setLabel(image.name);
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
			glm::ivec2(0, 0),
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
	
	impl->generateMipmap();
}

static SharedPtr<Texture> loadTexture(gltf::data &data, gltf::texture &texture) {
	if (texture.impl != nullptr) //< Should this be marked as Likely? Texture loading is fairly lazy, in the sense we don't do any manual checking of existence up until now. Materials share textures quite often.
		return texture.impl;
	
	texture.impl = std::make_shared<Texture>(gl::TextureTarget::Texture2D);
	
	auto &[mag_filter, min_filter, wrap_s_mode, wrap_t_mode] = data.samplers[texture.sampler];
	gltf::image const &image = data.images[texture.source];

	SharedPtr<Texture> const impl = texture.impl;
	if (image.is_dds) loadDDS(image, impl);
	else if (image.is_ktx2) loadKTX2(image, impl);
	else loadPNG(image, impl);

	impl->setFilter(min_filter, mag_filter);
	impl->setWrapMode(gl::TextureWrapMode::ClampToEdge, wrap_s_mode, wrap_t_mode);
	impl->enableAnisotropicFiltering();
	return impl;
}

static SharedPtr<Texture> loadTexture(gltf::data &data, gltf::id const texture_id) {
	//< Bounds check.
	std::size_t const texture_index = static_cast<std::size_t>(texture_id);
	if (texture_index >= data.textures.size()) {
		assert(false && "Texture ID out of bounds");
		return nullptr;
	}
	return loadTexture(data, data.textures[texture_index]);
}

static SharedPtr<Material> loadMaterial(gltf::data &data, gltf::material &gltf_material) {
	if (gltf_material.impl != nullptr)
		return gltf_material.impl;

	auto const mtl = std::make_shared<Material>();
	
	mtl->roughness_ = gltf_material.pbr_metallic_roughness.roughness_factor;
	mtl->metallic_  = gltf_material.pbr_metallic_roughness.metallic_factor;
	mtl->emissive_color_mod_ = gltf_material.emissive_factor;
	
	if (gltf_material.pbr_metallic_roughness.base_color_texture.exists) {
		gltf::id const texture_id = gltf_material.pbr_metallic_roughness.base_color_texture.index;
		SharedPtr<Texture> const impl = loadTexture(data, texture_id);
		mtl->setDiffuse(impl, gltf_material.pbr_metallic_roughness.base_color_factor);
	}
	
	if (gltf_material.pbr_metallic_roughness.metallic_roughness_texture.exists) {
		gltf::id const texture_id = gltf_material.pbr_metallic_roughness.metallic_roughness_texture.index;
		SharedPtr<Texture> const impl = loadTexture(data, texture_id);
		mtl->orm_ = impl;
	}
	
	if (gltf_material.normal_texture.exists) {
		gltf::id const texture_id = gltf_material.normal_texture.index;
		SharedPtr<Texture> const impl = loadTexture(data, texture_id);
		mtl->normal_ = impl;
	}

	if (gltf_material.emissive_texture.exists) {
		gltf::id const texture_id = gltf_material.emissive_texture.index;
		SharedPtr<Texture> const impl = loadTexture(data, texture_id);
		mtl->emissive_ = impl;
	}

	gltf_material.impl = mtl;

	return mtl;
}

static SharedPtr<Material> loadMaterial(gltf::data &data, u32 const material_id) {
	std::size_t const material_index = static_cast<std::size_t>(material_id);
	if (material_index >= data.materials.size()) {
		assert(false && "Material ID out of bounds");
		return nullptr;
	}

	gltf::material &gltf_material = data.materials[material_index];
	return loadMaterial(data, gltf_material);
}


#ifdef GLTF_USE_MANY_BUFFERS
#define SetupAttribute(N0,N1,C,D,E,F) applyAccessorAsAttribute(C,D,E,F, views)
#else
#define SetupAttribute(A,B,D,E,F,G,H,I) applyAccessorAsAttributeSingleBufferUnskinned(A,B,D,E,F,G,H,I)
#endif

AABB Mesh::processPrimitiveAttribs(gltf::data &data, SharedPtr<VertexArray> const &vertex_array, gltf::primitive const &primitive, Vec<SharedPtr<Buffer>> &views) {
#ifndef GLTF_USE_MANY_BUFFERS
	_STD vector<StandardVertex> vertex_buffer_;
	//_STD size_t vertex_size_ = 0;
	_STD size_t count_ = 0;

	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		//vertex_size_ += gltf::componentsForType(accessor.type()) * gltf::sizeForComponentType(accessor.componentType());
		count_ = std::max(count_, accessor.count());
	}
	auto const buf = _STD make_shared<Buffer>();
	vertex_buffer_.resize(count_);
#endif


	gltf::id index_accessor = primitive.indices;
	std::optional<gltf::id> position_accessor;
	std::optional<gltf::id> normal_accessor;
	std::optional<gltf::id> uv_accessor;
	
	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		gltfDebugPrintf("Accessor of name %s", name.c_str());
// ReSharper disable CppAssignedValueIsNeverUsed
		switch (hash(name)) {
			case hash("POSITION"):
				position_accessor = accessor_id;
				gltfDebugPrint("POSITION attribute identified");
				SetupAttribute(vertex_buffer_,0,data,0,vertex_array,accessor);
				break;
			case hash("NORMAL"):
				normal_accessor = accessor_id;
				gltfDebugPrint("NORMAL attribute identified");
				SetupAttribute(vertex_buffer_, 12, data, 1, vertex_array, accessor);
				break;
			case hash("TEXCOORD_0"):
				uv_accessor = accessor_id;
				gltfDebugPrint("TEXCOORD_0 attribute identified");
				SetupAttribute(vertex_buffer_, 24, data, 3, vertex_array, accessor);
				break;
			case hash("TEXCOORD_1"):
				gltfDebugPrint("TEXCOORD_1 attribute identified");
				SetupAttribute(vertex_buffer_, 48, data, 4, vertex_array, accessor);
				break;
			case hash("TANGENT"):
				gltfDebugPrint("TANGENT attribute identified");
				SetupAttribute(vertex_buffer_, 32, data, 2, vertex_array, accessor);
				break;
			case hash("JOINTS_0"):
				gltfDebugPrint("JOINTS_0 attribute identified");
				SetupAttribute(vertex_buffer_, 44, data, 4, vertex_array, accessor);
				break;
			case hash("WEIGHTS_0"):
				gltfDebugPrint("WEIGHTS_0 attribute identified");
				SetupAttribute(vertex_buffer_, 48, data, 5, vertex_array, accessor);
				break;
			default: break;
		}
// ReSharper restore CppAssignedValueIsNeverUsed
		gpu_check;
	}

#if 0
	if (position_accessor.has_value() && uv_accessor.has_value() && index_accessor != -1)
		computeTangents(
			data,
			vertex_array,
			primitive,
			index_accessor,
			position_accessor.value(),
			normal_accessor.value(),
			uv_accessor.value());
#endif

#ifndef GLTF_USE_MANY_BUFFERS
	buf->upload(
		sizeof(StandardVertex) * vertex_buffer_.size(),
		vertex_buffer_.data(),
		gl::BufferUsageARB::StaticDraw
	);
	gpu_check;
	
	buffers_.push_back(buf);
	vertex_array->setVertexBuffer(0, *buf, sizeof(StandardVertex), 0);

	return processAABB(vertex_buffer_);
#else
	return AABB(vec3(0.0f), vec3(0.0f)); // placeholder
#endif
}

void Mesh::applyAccessorAsAttribute(gltf::data const &data, i32 const index, _STD shared_ptr<VertexArray> const &vertex_array, gltf::accessor const &accessor, Vec<SharedPtr<Buffer>> &views) {
#ifdef GLTF_USE_MANY_BUFFERS
	VertexAttribute_t attrib{};
	attrib.offset = 0;

	attrib.size = gltf::componentsForType(accessor.type());

	size_t const size = gltf::sizeForComponentType(accessor.componentType());
	attrib.type = gltf::gpuComponentTypeFromGltfComponentType(accessor.componentType());
	
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];

	if (accessor.offset() > 0) {
		gpuDebugf("[Attrib %d] bufferView=%d bv.offset=%llu bv.length=%llu accessor.offset=%llu count=%llu type=%s\n",
			index,
			accessor.bufferView(),
			buffer_view.offset,
			buffer_view.length,
			accessor.offset(),  // <-- this is the critical one
			accessor.count(),
			gltf::to_string(accessor.type())
		);
	}

	assert(gltf_buffer.length() > 0);
	assert(buffer_view.offset + buffer_view.length <= gltf_buffer.length());

	if (index == 2) {
		gpuDebugf("[TANGENT prim] bv=%d offset=%llu len=%llu | first: %.4f %.4f %.4f %.4f | second: %.4f %.4f %.4f %.4f\n",
			accessor.bufferView(),
			buffer_view.offset,
			buffer_view.length,
			t[0], t[1], t[2], t[3],
			t[4], t[5], t[6], t[7]
		);
	}

	SharedPtr<Buffer> &buffer = views[accessor.bufferView()];

	if (buffer.get() == nullptr) {
		buffer.reset(new Buffer());
		assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);
		
		size_t const data_offset = buffer_view.offset;
		size_t const data_length = buffer_view.length;

		buffer->allocStorage(data_length, &gltf_buffer.data()[data_offset], std::nullopt);

		buffers_.push_back(buffer); // It will not be in the local buffers storage if it hasn't existed until now.
		gpu_check;
	}
	else {
		bool has_buffer_already = false;
		for (SharedPtr<Buffer> const &b : buffers_) {
			if (b == buffer) {
				has_buffer_already = true;
				break;
			}
		}

		if (!has_buffer_already)
			buffers_.push_back(buffer);
	}

	// Calculate stride
	i32 const stride = static_cast<i32>(accessor.stride());

	vertex_array->setVertexBuffer(
		index,
		*buffer,
		buffer_view.stride == 0 ? stride : static_cast<i32>(buffer_view.stride),
		static_cast<i64>(accessor.offset())
	);
	
	gpu_check;

	vertex_array->bind();

	size_t const binding = index;
	
	gpu_check;

	gpuDebugf("Generic attribute created for vao, index %i size %llu len %u and stride is %d", index, size, buffer_view.length, attrib.size * static_cast<i32>(size));

	attrib.index = index;
	attrib.binding = static_cast<i32>(binding); // its convenient ig
	attrib.stride = static_cast<i32>(buffer_view.stride == 0 ? static_cast<std::size_t>(attrib.size) * size : buffer_view.stride);
	//attrib.offset = static_cast<u32>(accessor.offset());
	vertex_array->setAttribute(attrib);
	gpu_check;
#else
	
#endif
}

void Mesh::applyAccessorAsAttributeSingleBuffer(
	size_t &file_buffer_id,
	std::fstream &file,
	std::vector<skinned_vertex> &buffer,
	size_t const offset,
	gltf::data const &data,
	i32 const index,
	SharedPtr<VertexArray> const &vertex_array,
	gltf::accessor const &accessor
) {
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];

	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

	auto const raw_data = &gltf_buffer.data()[buffer_view.offset];
	for (size_t i = 0; i < accessor.count(); ++i) {
		switch (index) {
			case 0:
				buffer[i].position = reinterpret_cast<glm::vec3 const *>(raw_data)[i];
				break;
			case 1:
				buffer[i].normal = reinterpret_cast<glm::vec3 const *>(raw_data)[i];
				break;
			case 3:
				buffer[i].texcoord0 = reinterpret_cast<glm::vec2 const *>(raw_data)[i];
				break;
			case 4:
				buffer[i].joints0 = reinterpret_cast<uint32_t const *>(raw_data)[i];
				break;
			case 5:
				buffer[i].weights0 = reinterpret_cast<glm::vec4 const *>(raw_data)[i];
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
	attrib.stride = 64;
	attrib.normalized = false;
	attrib.index = index;
	vertex_array->setAttribute(attrib);
	gpu_check;
}

void Mesh::applyAccessorAsElementBuffer(gltf::data const &data, _STD shared_ptr<VertexArray> const &vertex_array, gltf::accessor const &accessor, Vec<SharedPtr<Buffer>> &views) {
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	SharedPtr<Buffer> &buffer = views[accessor.bufferView()];
	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

	if (buffer.get() == nullptr) {
		buffer.reset(new Buffer());
		assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);
		
		size_t const data_offset = buffer_view.offset;
		size_t const data_length = buffer_view.length;
	
		buffer->allocStorage(
			data_length, &gltf_buffer.data()[data_offset],
			std::nullopt
		);

		buffers_.push_back(buffer); // It will not be in the local buffers storage if it hasn't existed until now.
	}
	else {
		bool has_buffer_already = false;
		for (SharedPtr<Buffer> const &b : buffers_) {
			if (b == buffer) {
				has_buffer_already = true;
				break;
			}
		}

		if (!has_buffer_already)
			buffers_.push_back(buffer);
	}
	
	vertex_array->bind();

	vertex_array->elements_count = accessor.count();
	vertex_array->offset_of_elements = accessor.offset(); //< offset is in bytes.
	
	vertex_array->draw_elements_type = gl::DrawElementsType::UnsignedShort;
	vertex_array->setElementBuffer(*buffer);
	
	gltfDebugPrintf("Element buffer applied with %llu elements", accessor.count());
}

void Mesh::processMesh(gltf::data &data, gltf::mesh const &mesh, Vec<SharedPtr<Buffer>> &views) {
	char i = '0';
	u32 prim_id = 0u;
	
	for (gltf::primitive const &primitive : mesh.primitives) {
		auto const vertex_array = _STD make_shared<VertexArray>();// = primitives_.back();
		vertex_array->bind();

		_STD string name = mesh.name + "#" + i;
		vertex_array->setLabel(name);
		
		AABB const aabb = processPrimitiveAttribs(data, vertex_array, primitive, views);

		if (primitive.indices != -1) {
			assert(data.accessors.size() > static_cast<_STD size_t>(primitive.indices));
			gltf::accessor &accessor = data.accessors[primitive.indices];
			applyAccessorAsElementBuffer(data, vertex_array, accessor, views);
		}
		
		gpu_check;

		u32 const material_value = primitive.material;
		
		primitives_.push_back({
			.vertex_array = vertex_array,
			//< GLTF 2.0 spec. doesn't require you to define a material for a primitive, and if you don't then the material is supposed to be a default one. We can just treat it as a null material and handle it in the shader.
			.material = static_cast<std::size_t>(material_value) < data.materials.size() ? loadMaterial(data, material_value) : nullptr, //< If bigger than the materials list then it's most likely UINT32_MAX and therefore is unspecified.
			.aabb_ = aabb
		});
		
		prim_id++;
		i++;
	}
}