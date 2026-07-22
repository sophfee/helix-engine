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


// ─────────────────────────────────────────────────────────────────────────────
// *** CPP  —  add this include near the top of mesh.cpp
// ─────────────────────────────────────────────────────────────────────────────

#include "model_manager.hpp"
#include "primitive.hpp"
#include "engine/engine.h"
#include "engine/main-loop.hpp"
#include "engine/thread_pool.hpp"
#include "mikktspace/mikktspace.h"
#include "renderers/renderer.hpp"

struct PrimAttribResult {
    AABB                           aabb;
    std::future<std::vector<vec4>> tangent_future; // invalid when not needed
};

struct MikktUserData {
	vec3 const *positions  = nullptr;
	vec3 const *normals    = nullptr;
	vec2 const *texcoords  = nullptr;
	u16  const *indices    = nullptr;
	u32         index_count = 0;
	std::vector<vec4> tangents_unindexed;
};

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
// ── MikkTSpace callbacks (all static, no class involvement) ─────────────────

static int mkkt_getNumFaces(SMikkTSpaceContext const *ctx) {
    auto const *ud = static_cast<MikktUserData const *>(ctx->m_pUserData);
    return static_cast<int>(ud->index_count / 3);
}

static int mkkt_getNumVertsOfFace(SMikkTSpaceContext const *, int) {
    return 3; // triangles only
}

static void mkkt_getPosition(SMikkTSpaceContext const *ctx, float out[], int const iFace, int const iVert) {
    auto const *ud = static_cast<MikktUserData const *>(ctx->m_pUserData);
    vec3 const &p  = ud->positions[ud->indices[iFace * 3 + iVert]];
    out[0] = p.x; out[1] = p.y; out[2] = p.z;
}

static void mkkt_getNormal(SMikkTSpaceContext const *ctx, float out[], int const iFace, int const iVert) {
    auto const *ud = static_cast<MikktUserData const *>(ctx->m_pUserData);
    vec3 const &n  = ud->normals[ud->indices[iFace * 3 + iVert]];
    out[0] = n.x; out[1] = n.y; out[2] = n.z;
}

static void mkkt_getTexCoord(SMikkTSpaceContext const *ctx, float out[], int const iFace, int const iVert) {
    auto const *ud = static_cast<MikktUserData const *>(ctx->m_pUserData);
    vec2 const &uv = ud->texcoords[ud->indices[iFace * 3 + iVert]];
    out[0] = uv.x; out[1] = uv.y;
}

static void mkkt_setTSpaceBasic(SMikkTSpaceContext const *ctx,
                                float const fvTangent[], float const fSign,
                                int const iFace, int const iVert) {
    auto *ud = static_cast<MikktUserData *>(ctx->m_pUserData);
    ud->tangents_unindexed[iFace * 3 + iVert] =
        vec4(fvTangent[0], fvTangent[1], fvTangent[2], fSign);
}

static std::vector<vec4> computeTangentsMikkt(
    gltf::data    const &data,
    gltf::id const index_accessor_id,
    gltf::id const position_accessor_id,
    gltf::id const normal_accessor_id,
    gltf::id const texcoord_accessor_id)
{
    MikktUserData ud;

	// We need unwelded geometry
    ud.positions   = accessorPtr<vec3>(data, position_accessor_id);
    ud.normals     = accessorPtr<vec3>(data, normal_accessor_id);
    ud.texcoords   = accessorPtr<vec2>(data, texcoord_accessor_id);
    ud.indices     = accessorPtr<u16> (data, index_accessor_id);
    ud.index_count = static_cast<u32>(data.accessors[index_accessor_id].count());
    ud.tangents_unindexed.resize(ud.index_count);

    SMikkTSpaceInterface iface;
    iface.m_getNumFaces          = mkkt_getNumFaces;
    iface.m_getNumVerticesOfFace = mkkt_getNumVertsOfFace;
    iface.m_getPosition          = mkkt_getPosition;
    iface.m_getNormal            = mkkt_getNormal;
    iface.m_getTexCoord          = mkkt_getTexCoord;
    iface.m_setTSpaceBasic       = mkkt_setTSpaceBasic;
    iface.m_setTSpace            = nullptr; // basic is sufficient for normal mapping

    SMikkTSpaceContext const ctx{ &iface, &ud };
    tbool const ok = genTangSpaceDefault(&ctx);
    assert(ok && "MikkTSpace tangent generation failed");
    (void)ok;

    return std::move(ud.tangents_unindexed);
}

static void uploadTangentBuffer(
    Vec<SharedPtr<Buffer>>       &buffers_out,   // Mesh::buffers_
    SharedPtr<VertexArray> const &vertex_array,
    std::vector<vec4>             tangents)       // moved-in
{
    auto tangent_buffer = std::make_shared<Buffer>();
    tangent_buffer->allocate(
        tangents.size() * sizeof(vec4),
        tangents.data(),
        std::nullopt
    );
    buffers_out.push_back(tangent_buffer);

    u32 const binding = vertex_array->vertex_buffer_count++;

    vertex_array->setVertexBuffer(
        binding,
        *tangent_buffer,
        sizeof(vec4),  // stride
        sizeof(vec4)   // offset
    );
    vertex_array->setAttribute({
        .index      = 2,                          // TANGENT lives at attrib slot 2
        .binding    = static_cast<i32>(binding),
        .size       = 4,
        .stride     = sizeof(vec4),
        .offset     = 0,
        .type       = EComponentType::SINGLE_FLOAT,
        .normalized = false
    });
}

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

void Mesh::drawSubMesh(RenderPassInfo const &info, _STD size_t const submesh) {
	auto const &[material, __, _, aabb] = primitives_[submesh];
	if (material) { //< Not really likely or unlikely I think.
		material->bind(info);
	}
}

void Mesh::drawAllSubMeshes(RenderPassInfo const &info) {
	if (mesh_count <= 0) assert(false);
	using enum gl::BufferTargetARB;
	switch (info.pass) {
		case RenderPassType::Normal: {
			vertex_array.bind();
			
			glMultiDrawElementsIndirectCount(
				GL_TRIANGLES,
				GL_UNSIGNED_SHORT,
				(void*)(sizeof(DrawElementsIndirectCommand) * draw_command_offset), // offset into draw_command_buffer_
				sizeof(u32) * draw_command_count_offset,       // offset into draw_command_count_buffer_
				static_cast<GLsizei>(mesh_count),
				0
			);
		}
	}
}

void Mesh::addPrimitive(SharedPtr<VertexArray> const &vertex_array, SharedPtr<Material> const &material, AABB const &aabb) {
	//primitives_.push_back(MeshPrimitive{vertex_array, material, {}, {}, aabb});
}
void Mesh::addBuffer(SharedPtr<Buffer> const &buffer) {
	buffers_.push_back(buffer);
}
void Mesh::setMaterial(std::size_t const index, SharedPtr<Material> const &material) {
	primitives_[index].material = material;
}

bool Mesh::skinned() const {
	return false;
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
	//processMesh(data, mesh);
	auto ssbo_inv_bind_matrices = _STD make_shared<Buffer>();
	//ssbo_inv_bind_matrices->allocate(skin)
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
				internal_format = gl::InternalFormat::Rgb8;
				pixel_format = gl::PixelFormat::Rgb;
			}
			else {
				internal_format = gl::InternalFormat::Rgb8;
				pixel_format = gl::PixelFormat::Rgb;
			}
			break;
		case 4:
			if (compressed) {
				internal_format = gl::InternalFormat::Rgba8;
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
	// The loader may close the file on its own.
	if (F) assert(fclose(F) == 0);
}

static void loadKTX2(gltf::image const &image, std::shared_ptr<Texture> const &impl) {
	auto const ktx2 = image.ktx2_texture;
	Error const res = ktx::textureLoad(ktx2, impl->texture_object_);
	assert(res == OK);
	impl->generateMipmap();
	
	ktxTexture_Destroy(ktx2);
}

static void my_png_err(png_structp png_ptr, char const *message) {
	std::cout << "png err: " << message << '\n';
}

static void my_png_warn(png_structp png_ptr, char const *message) {
	std::cout << "png warn: " << message << '\n';
}


static void loadPNGAsync_Inner(int h, void *output, gltf::image const &image, std::shared_ptr<Texture> const &impl) {
}

static std::future<void> loadPNGAsync(Mesh &mesh, gltf::image const &image, std::shared_ptr<Texture> impl) {
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
		sizei_t const alloc_size = static_cast<sizei_t>(rowbytes * h);

		::Buffer const *pixelUnpack;
		void* data;
		Engine::singleton()->addLazyTaskToMainThreadQueue([&] {
			using enum MapBufferAccessMask;
			pixelUnpack = AsyncTextureBank::singleton()->checkout(alloc_size);
			if (pixelUnpack == nullptr)
				return false; // Don't remove this task, because something ain't right! Let it go around again.
			data = pixelUnpack->mapRange(0, alloc_size,  MapPersistentBit | MapWriteBit);
			return true;
		}).get();
		
		std::vector<png_bytep> rowPointers(h);
		for (int i = 0; i < h; i++) {
			rowPointers[i] = (png_bytep)data + i * rowbytes;
		}
		png_read_image(png_ptr, rowPointers.data());

		Engine::singleton()->addLazyTaskToMainThreadQueue([&pixelUnpack, w, h, channels, impl, &mesh] {
			using enum BufferTargetARB;
			using enum PixelType;
			assert(pixelUnpack->unmap());
			pixelUnpack->bind(PixelUnpackBuffer);
			InternalFormat internal_format;
			PixelFormat pixel_format;
			channelsToInternalFormat(channels, false, internal_format, pixel_format);
			impl->allocate(ivec2(w, h), 1, internal_format);
			impl->uploadImage2D(
				nullptr,
				0,
				ivec2(0, 0),
				ivec2(w, h),
				pixel_format,
				UnsignedByte
			);
			impl->generateMipmap();
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			AsyncTextureBank::singleton()->checkin(pixelUnpack);
			return true;
		}).get();
		
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(f);

		std::cout << "Finished loading PNG " <<  uri << " asynchronously.\n";
	});
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
	
	impl->generateMipmap();
}

static SharedPtr<Texture> loadTexture(Mesh &mesh, gltf::data &data, gltf::texture &texture) {
	if (texture.impl != nullptr) //< Should this be marked as Likely? Texture loading is fairly lazy, in the sense we don't do any manual checking of existence up until now. Materials share textures quite often.
		return texture.impl;
	
	texture.impl = std::make_shared<Texture>(gl::TextureTarget::Texture2D);
	
	auto &[mag_filter, min_filter, wrap_s_mode, wrap_t_mode] = data.samplers[texture.sampler];
	gltf::image const &image = data.images[texture.source];

	SharedPtr<Texture> const impl = texture.impl;
	switch (image.image_type) {
		case gltf::image_type_dds:
			loadDDS(image, impl);
			break;
		case gltf::image_type_ktx2:
			loadKTX2(image, impl);
			break;
		case gltf::image_type_png:
			mesh.async_tasks_.push_back(loadPNGAsync(mesh, image, impl));
			break;
		case gltf::image_type_generic:
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


#ifdef GLTF_USE_MANY_BUFFERS
#define SetupAttribute(N0,N1,C,D,E,F) applyAccessorAsAttribute(C,D,E,F, views)
#else
#define SetupAttribute(A,B,D,E,F,G,H,I) applyAccessorAsAttributeSingleBufferUnskinned(A,B,D,E,F,G,H,I)
#endif

PrimAttribResult  Mesh::processPrimitiveAttribs(gltf::data &data, SharedPtr<VertexArray> const &vertex_array, gltf::primitive const &primitive, Vec<SharedPtr<Buffer>> &views) {
#ifndef GLTF_USE_MANY_BUFFERS
	_STD vector<Vertex> vertex_buffer_;
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

	bool has_tangent = false;
	
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
			case hash("TANGENT"):
				has_tangent = true;
				gltfDebugPrint("TANGENT attribute identified");
				SetupAttribute(vertex_buffer_, 32, data, 2, vertex_array, accessor);
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
		sizeof(Vertex) * vertex_buffer_.size(),
		vertex_buffer_.data(),
		gl::BufferUsageARB::StaticDraw
	);
	gpu_check;
	
	buffers_.push_back(buf);
	vertex_array->setVertexBuffer(0, *buf, sizeof(Vertex), 0);

	return processAABB(vertex_buffer_);
#else
	std::future<std::vector<vec4>> tangent_future;

	if (!has_tangent
		&& position_accessor.has_value()
		&& normal_accessor.has_value()
		&& uv_accessor.has_value()
		&& index_accessor != -1)
	{
		tangent_future = std::async(
			std::launch::async,
			[&data,
			 ia  = index_accessor,
			 pa  = position_accessor.value(),
			 na  = normal_accessor.value(),
			 uva = uv_accessor.value()]() -> std::vector<vec4> {
				return computeTangentsMikkt(data, ia, pa, na, uva);
			});
	}

	return PrimAttribResult{ AABB(vec3(0),vec3(0)), std::move(tangent_future) };
#endif
}

template <typename T, std::size_t OFFSET>
static void iterate(gltf::data &data, Vec<Vertex> &out_vertices, gltf::id const acc, std::size_t const count) {
	std::span<T> accessor_span = accessorSpan<T>(data, acc);
	for (std::size_t i = 0; i < count; ++i)
		*(T*)(&out_vertices[i] + OFFSET) = accessor_span[i];
}

template <typename T, std::size_t OFFSET>
static void iterate(gltf::data &data, Vec<Vertex> &out_vertices, gltf::id const acc, std::size_t const count, auto fun) {
	std::span<T> accessor_span = accessorSpan<T>(data, acc);
	for (std::size_t i = 0; i < count; ++i) {
		*(T*)(&out_vertices[i] + OFFSET) = accessor_span[i];
		fun(*(T*)(&out_vertices[i] + OFFSET));
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
				position_vector.insert(position_vector.end(), positions.begin(), positions.end());
				for (vec3 const &pos : positions) { //< calculate min and max
					bounds_min.x = std::min(bounds_min.x, pos.x);
					bounds_min.y = std::min(bounds_min.y, pos.y);
					bounds_min.z = std::min(bounds_min.z, pos.z);
					bounds_max.x = std::max(bounds_max.x, pos.x);
					bounds_max.y = std::max(bounds_max.y, pos.y);
					bounds_max.z = std::max(bounds_max.z, pos.z);
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
	mesh.instanceCount  = 1;
	mesh.vertexCount   = static_cast<u32>(count_);
	
	return mesh;
}

void Mesh::applyAccessorAsAttribute(gltf::data const &data, i32 const index, _STD shared_ptr<VertexArray> const &vertex_array, gltf::accessor const &accessor, Vec<SharedPtr<Buffer>> &views) {
#ifdef GLTF_USE_MANY_BUFFERS
	VertexArrayAttribute attrib{};
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
		//gpuDebugf("[TANGENT prim] bv=%d offset=%llu len=%llu | first: %.4f %.4f %.4f %.4f | second: %.4f %.4f %.4f %.4f\n",
		//	accessor.bufferView(),
		//	buffer_view.offset,
		//	buffer_view.length,
		//	t[0], t[1], t[2], t[3],
		//	t[4], t[5], t[6], t[7]
		//);
	}

	SharedPtr<Buffer> &buffer = views[accessor.bufferView()];

	if (buffer.get() == nullptr) {
		buffer.reset(new Buffer());
		assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);
		
		size_t const data_offset = buffer_view.offset;
		size_t const data_length = buffer_view.length;

		buffer->allocate(data_length, &gltf_buffer.data()[data_offset], std::nullopt);

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

	auto binding = vertex_array->vertex_buffer_count++;

	vertex_array->setVertexBuffer(
		binding,
		*buffer,
		buffer_view.stride == 0 ? stride : static_cast<i32>(buffer_view.stride),
		static_cast<i64>(accessor.offset())
	);
	
	gpu_check;

	vertex_array->bind();
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
				buffer[i].position = reinterpret_cast<vec3 const *>(raw_data)[i];
				break;
			case 1:
				buffer[i].normal = reinterpret_cast<vec3 const *>(raw_data)[i];
				break;
			case 3:
				buffer[i].texcoord0 = reinterpret_cast<vec2 const *>(raw_data)[i];
				break;
			case 4:
				buffer[i].joints0 = reinterpret_cast<uint32_t const *>(raw_data)[i];
				break;
			case 5:
				buffer[i].weights0 = reinterpret_cast<vec4 const *>(raw_data)[i];
				break;
			default:
				break;
		}
	}
	VertexArrayAttribute attrib;
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

void Mesh::applyAccessorAsElementBuffer(gltf::data const &data, _STD shared_ptr<VertexArray> const &vertex_array, gltf::accessor const &accessor, Vec<SharedPtr<Buffer>> &views, Vec<u16> &packed_indices) {
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	SharedPtr<TypedBuffer<u16>> buffer = std::static_pointer_cast<TypedBuffer<u16>>(views[accessor.bufferView()]);
	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);	

	if (buffer == nullptr) {
		buffer.reset(new TypedBuffer<u16>());
		assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);
		
		std::size_t const data_offset = buffer_view.offset;
		std::size_t const data_length = buffer_view.length;
		
		buffer->allocateElements(data_length / sizeof(u16), (u16 const*)&gltf_buffer.data()[data_offset], std::nullopt);
		buffers_.push_back(buffer); // It will not be in the local buffers storage if it hasn't existed until now.

		packed_indices.resize(accessor.count());
		auto const indices = reinterpret_cast<u16 const*>(&gltf_buffer.data()[data_offset]);
		std::memcpy(packed_indices.data(), indices, accessor.count() * sizeof(u16));
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
	
	Vec<vec3> positions;
	Vec<vec3> normals;
	Vec<vec4> tangents;
	Vec<vec2> texcoord0;
	Vec<vec2> texcoord1;
	Vec<u16> indices;
	
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
			case RendererType::FORWARD_MULTI: {
				std::size_t const start_vertices = positions.size();
				std::size_t start_indices = indices.size();
				GpuMesh gpu_mesh = processPrimitiveAttribsIntoSeparateVector(data, primitive, positions, normals, tangents, texcoord0, texcoord1);

				std::size_t index_count;
				if (primitive.indices != -1) {
					assert(data.accessors.size() > static_cast<std::size_t>(primitive.indices));
					std::span<u16> indicesData = accessorSpan<u16>(data, primitive.indices);
					indices.insert(indices.end(), indicesData.begin(), indicesData.end());
					index_count = std::distance(indicesData.begin(), indicesData.end());
				}

				gpu_check;

				u32 const mat_id = primitive.material;

				gpu_mesh.vertexCount  = static_cast<i32>(positions.size() - start_vertices);
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
			case RendererType::FORWARD:
				break;
			default: 
				assert(false && "Unsupported renderer type");
				break;
		}
	}

	for (size_t i = 0; i < positions.size(); ++i) {
		printf("Position %f %f %f\n", positions[i][0], positions[i][1], positions[i][2]);
	}
	
	primitives_.reserve(primitives_.size() + records.size());

	position_buffer_  = positions;
	normal_buffer_    = normals;
	tangent_buffer_   = tangents;
	texcoord0_buffer_ = texcoord0;
	texcoord1_buffer_ = texcoord1;
	index_buffer_     = indices;
	
	vertex_array.setLabel(mesh.name + "_VAO");
	vertex_array.bind();

	vertex_array.setVertexBuffer(0, position_buffer_, 12, 0);
	position_buffer_.setLabel(mesh.name + "_position");
	vertex_array.setAttribute({
		.index   = 0,
		.binding = 0,
		.size    = 3,
		.stride  = 12,
		.offset  = 0,
		.type    = EComponentType::SINGLE_FLOAT,
		.normalized = false
	});

	vertex_array.setVertexBuffer(1, normal_buffer_, 12, 0);
	normal_buffer_.setLabel(mesh.name + "_normal");
	vertex_array.setAttribute({
		.index   = 1,
		.binding = 1,
		.size    = 3,
		.stride  = 12,
		.offset  = 0,
		.type    = EComponentType::SINGLE_FLOAT,
		.normalized = false
	});

	vertex_array.setVertexBuffer(2, tangent_buffer_, 16, 0);
	tangent_buffer_.setLabel(mesh.name + "_tangents");
	vertex_array.setAttribute({
		.index   = 2,
		.binding = 2,
		.size    = 4,
		.stride  = 16,
		.offset  = 0,
		.type    = EComponentType::SINGLE_FLOAT,
		.normalized = false
	});

	vertex_array.setVertexBuffer(3, texcoord0_buffer_, 8, 0);
	texcoord0_buffer_.setLabel(mesh.name + "_texcoord0");
	vertex_array.setAttribute({
		.index = 3,
		.binding = 3,
		.size = 2,
		.stride = 8,
		.offset = 0,
		.type = EComponentType::SINGLE_FLOAT,
		.normalized = false
	});

	vertex_array.setVertexBuffer(4, texcoord1_buffer_, 8, 0);
	texcoord1_buffer_.setLabel(mesh.name + "_texcoord1");
	vertex_array.setAttribute({
		.index    =  4,
		.binding  =  4,
		.size     =  2,
		.stride   =  8,
		.offset   =  0,
		.type = EComponentType::SINGLE_FLOAT,
		.normalized = false
	});
	
	vertex_array.setElementBuffer(index_buffer_);
	index_buffer_.setLabel(mesh.name + "_indices");
	
	mesh_count = static_cast<u32>(meshes.size());
	
	printf("Finished loading mesh %s\n", mesh.name.c_str());
}