// ReSharper disable CppCStyleCast
// ReSharper disable CppClangTidyCertErr33C
#include "mesh.hpp"
#include "gltf.h"
#include "libpng/png.h"
#include <Windows.h>

#include <future>
#include <cassert>
#include <utility>

#include "placeholders.hpp"
#include "texture.h"
#include "util.hpp"
#include "khr/ktx.h"
#include "khr/ktx_ext.h"
#include "loaders/dds.hpp"

#define GLTF_USE_MANY_BUFFERS

#if 0

_STD size_t CRuntimeStruct::defineField(_STD size_t size, _STD size_t padding) {
	if (anythingAllocated())
		transmogrifyInnerData();

	if (padding > 0)
		fields_.push_back(padding); // empty field

	fields_.push_back(size);
	return size;
}
_STD size_t CRuntimeStruct::getFieldSize(_STD size_t const field) const {
	return fields_[field];
}


bool CRuntimeStruct::anythingAllocated() const {
	return data_.size() == 0 || data_.empty();
}
#endif

CSkin::CSkin() {
}

CSkin::~CSkin() {
}

Mesh::Mesh() {
}

Mesh::Mesh(gltf::data &data) : material_info_(data.materials) {
	//for (gltf::mesh &mesh : data.meshes)
	//	processMesh(data, mesh);
	//primitives_.back().vertex_array->unbind();
	//processTextures(data);
}

Mesh::Mesh(gltf::data &data, _STD size_t const mesh_id) : is_skinned_(false), material_info_(data.materials) {
	//processMesh(data, data.meshes[mesh_id]);
	//processTextures(data);
}
Mesh::Mesh(gltf::data &data, std::size_t mesh_id, Vec<SharedPtr<Buffer>> &views) {
	processMesh(data, data.meshes[mesh_id], views);
	processTextures(data);
}

Mesh::Mesh(gltf::data &data, _STD size_t const mesh_id, [[maybe_unused]] _STD size_t skin_id) : is_skinned_(true), material_info_(data.materials) {
	//processMesh(data, data.meshes[mesh_id]);
	processTextures(data);
}

Mesh::~Mesh() {
}

_STD size_t Mesh::subMeshCount() const {
	return primitives_.size();
}

void Mesh::drawSubMesh(RenderPassInfo const &info, _STD size_t const submesh) const {
	auto const &[vertex_array, aabb, material] = primitives_[submesh];
	_STD size_t const
		base_color_texture = material_info_[material].pbr_metallic_roughness.base_color_texture.index,
		metallic_roughness_texture = material_info_[material].pbr_metallic_roughness.metallic_roughness_texture.index,
		normal_texture = material_info_[material].normal_texture.index;

	bool has_any_missing_texture = false;
	i32 missing_texture_bound_to = 0;
	
	if (base_color_texture < textures_.size() && info.bind_albedo_texture) {
		if (textures_[base_color_texture] != nullptr) {
			if (textures_[base_color_texture]->isValid()) {
				textures_[base_color_texture]->bindTextureUnit(0);
				glUniform1i(3, 0);
				goto apply_metallic_roughness_texture;
			}
		}
	}

	rd::missing_texture->bindTextureUnit(0);
	has_any_missing_texture = true;
	missing_texture_bound_to = 0;
	glUniform1i(3, 0);

apply_metallic_roughness_texture:
	if (metallic_roughness_texture < textures_.size() && info.bind_orm_texture) {
		if (textures_[metallic_roughness_texture] != nullptr) {
			if (textures_[metallic_roughness_texture]->isValid()) {
				textures_[metallic_roughness_texture]->bindTextureUnit(1);
				glUniform1i(4, 1);
				goto apply_normal_texture;
			}
		}
	}

	if (!has_any_missing_texture) {
		has_any_missing_texture = true;
		missing_texture_bound_to = 1;
		rd::missing_texture->bindTextureUnit(1);
	}
	glUniform1i(4, missing_texture_bound_to);

apply_normal_texture:
	if (normal_texture < textures_.size() && info.bind_normal_texture) {
		if (textures_[normal_texture] != nullptr) {
			if (textures_[normal_texture]->isValid()) {
				textures_[normal_texture]->bindTextureUnit(2);
				glUniform1i(5, 2);
				goto draw_vertex_arrays;
			}
		}
	}

	if (!has_any_missing_texture) {
		has_any_missing_texture = true;
		missing_texture_bound_to = 2;
		rd::missing_texture->bindTextureUnit(2);
	}
	glUniform1i(5, missing_texture_bound_to);

draw_vertex_arrays:
	vertex_array->bind();
	vertex_array->draw();
	
	gpu_check;
}
void Mesh::drawAllSubMeshes(RenderPassInfo const &info) const {
	for (auto const &primitive : primitives_) {
		auto const vertex_array = primitive.vertex_array;
		
		u32 const material = primitive.material;

		if (material >= material_info_.size()) {
			vertex_array->bind();
			assert(!vertex_array->disposed());
			vertex_array->draw();
			gpu_check;
			continue;
		}
		
		_STD size_t const
			base_color_texture			= 	material_info_[material].pbr_metallic_roughness.base_color_texture.index,
			metallic_roughness_texture	= 	material_info_[material].pbr_metallic_roughness.metallic_roughness_texture.index,
			normal_texture				= 	material_info_[material].normal_texture.index;
		
		if (base_color_texture < textures_.size() && info.bind_albedo_texture) {
			if (textures_[base_color_texture] != nullptr) {
				//if (textures_[base_color_texture]->isValid())
				{
					textures_[base_color_texture]->bindTextureUnit(0);
					glUniform1i(3, 0);
				}
			}
		}
		
		if (metallic_roughness_texture < textures_.size() && info.bind_orm_texture) {
			if (textures_[metallic_roughness_texture] != nullptr) {
				if (textures_[metallic_roughness_texture]->isValid()) {
					textures_[metallic_roughness_texture]->bindTextureUnit(1);
					glUniform1i(4, 1);
				}
			}
		}

		if (normal_texture < textures_.size() && info.bind_normal_texture) {
			if (textures_[normal_texture] != nullptr) {
				if (textures_[normal_texture]->isValid()) {
					textures_[normal_texture]->bindTextureUnit(2);
					glUniform1i(5, 2);
				}
			}
		}
		
		vertex_array->bind();
		assert(!vertex_array->disposed());
		vertex_array->draw();
		gpu_check;
	}
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
		gltf::id normal_accessor,
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

		f32 normal_dot_tangent = glm::dot(normal, tangent);
		vec3 normal_x_dot_product = normal * normal_dot_tangent;
		// Gram-Schmidt orthogonalization of tangent against normal
		f32 ndott = glm::dot(normal, tangent);
		vec3 tangent_vector = glm::normalize(tangent - normal * ndott);

		// Handedness: sign of the triple product
		f32 handedness = (glm::dot(glm::cross(normal, tangent), binormal) < 0.0f) ? -1.0f : 1.0f;
		
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

namespace {
	std::vector<char> temp_alloc_buffer_(0);
}
constexpr auto alloc_block_step = 0x100000;
void Mesh::processMesh(gltf::data &data, gltf::mesh const &mesh, Vec<SharedPtr<Buffer>> &views) {
	char i = '0';
	std::fstream file(data.buffers[0].uri(), std::ios::binary);
	size_t file_buffer_id = 0ull;

	std::cout << views.size() << '\n';
	
	for (gltf::primitive const &primitive : mesh.primitives) {
		auto const vertex_array = _STD make_shared<VertexArray>();// = primitives_.back();
		vertex_array->bind();
		_STD string name = mesh.name + "#" + i;
		vertex_array->setLabel(name);
		
		AABB const aabb = processPrimitiveAttribs(file_buffer_id, file, data, vertex_array, primitive, views);

		if (primitive.indices != -1) {
			assert(data.accessors.size() > static_cast<_STD size_t>(primitive.indices));
			gltf::accessor &accessor = data.accessors[primitive.indices];
			applyAccessorAsElementBuffer(file_buffer_id, file, data, vertex_array, accessor, views);
		}
		gpu_check;

		/*
		Vec<vec3> bitangents(data.accessors[primitive.attributes[0].accessor].count());

		size_t n = 0;
		gltf::accessor &indices = data.accessors[primitive.indices];
		gltf::buffer_view &bv = data.buffer_views[indices.bufferView()];

		gltf::buffer &buffer = data.buffers[bv.buffer];
		
		for (size_t tri = 0; tri < indices.count(); tri += 3) {
			auto i0 = (u16)buffer[bv.offset + indices.offset() + tri * gltf::sizeForComponentType(indices.componentType())];
			auto i1 = (u16)buffer[bv.offset + indices.offset() + (tri + 1) * gltf::sizeForComponentType(indices.componentType())];
			auto i2 = (u16)buffer[bv.offset + indices.offset() + (tri + 2) * gltf::sizeForComponentType(indices.componentType())];
			
			n++;
		}
		*/

		u32 const material_value = primitive.material;
		
		primitives_.push_back({
			.vertex_array = vertex_array,
			.aabb_ = aabb,
			.material = material_value
		});
		i++;
	}
}

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

void Mesh::processTextures(gltf::data &data) {
	
	// finish handling those images, they've had time to actually load now :
	for (auto &[sampler, source, impl] : data.textures) {
		gltf::image &image = data.images[source];

		if (impl != nullptr) {
			textures_.push_back(impl);
			continue;
		}
		
		auto &[mag_filter, min_filter, wrap_s_mode, wrap_t_mode] = data.samplers[sampler];
		
		auto internal_format = gl::InternalFormat::Rgb8;
		auto pixel_format = gl::PixelFormat::Rgb;
		switch (image.channels) {
			case 1:
				internal_format = gl::InternalFormat::R8;
				pixel_format = gl::PixelFormat::Red;
				break;
			case 2:
				internal_format = gl::InternalFormat::Rg8;
				pixel_format = gl::PixelFormat::Rg;
				break;
			case 3:
				internal_format = gl::InternalFormat::CompressedRgbS3tcDxt1Ext;
				pixel_format = gl::PixelFormat::Rgb;
				break;
			case 4:
				
				pixel_format = gl::PixelFormat::Rgba;
				break;
		}
		std::string imageUid = ".local/img-cache/" + std::to_string(image.hash_value) + ".hltx";

		if (image.is_dds) {
			impl = _STD make_shared<Texture>(gl::TextureTarget::Texture2D);
			FILE *F = fopen(image.file.c_str(), "rb");
			std::string err = "";
			Error const res = DDS_UploadFromStdIO(F, impl->texture_object_, err);
			if (res != OK) __debugbreak();
			assert(res == OK);
			assert(fclose(F) == 0);
			impl->enableAnisotropicFiltering();
		}
		else if (image.is_ktx2) {
			auto const ktx2 = (ktxTexture*)image.ktx2_texture;
			
			impl = _STD make_shared<Texture>(gl::TextureTarget::Texture2D);
			Error const res = ktx::textureLoad(image.ktx2_texture, impl->texture_object_);
			assert(res == OK);
			ktxTexture_Destroy(ktx2);
			
		}
		else {
			impl = _STD make_shared<Texture>(gl::TextureTarget::Texture2D);
			impl->setLabel(image.name);
			assert(_CrtCheckMemory());
			impl->allocate(image.size, 1, internal_format);
			if (image.compressed) {
				impl->uploadImage2D(
					image.external_data->data(),
					0,
					glm::ivec2(0, 0),
					image.size,
					pixel_format,
					gl::PixelType::UnsignedByte
				);
				impl->setFilter(gl::TextureMinFilter::LinearMipmapLinear, gl::TextureMagFilter::Linear);
				impl->enableAnisotropicFiltering();
				impl->generateMipmap();
			} else if (image.external_data->data() != nullptr) {
				impl->uploadImage2D(
					image.external_data->data(),
					0,
					glm::ivec2(0, 0),
					image.size,
					pixel_format,
					gl::PixelType::UnsignedByte
				);
				impl->enableAnisotropicFiltering();
				impl->generateMipmap();
			
				assert(_CrtCheckMemory());
			
				image.external_data->clear();
				image.external_data->shrink_to_fit();

				glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
			
				std::vector<u8> compressed_data;
				impl->imageData(compressed_data, 0);

				_STD wstring wImageUid = stringToWideString(imageUid);
			
				FILE *compressed_data_file = fopen(imageUid.c_str(), "wb");
				assert(compressed_data_file != nullptr);
				u16 const size_data[2] { static_cast<u16>(image.size.x), static_cast<u16>(image.size.y) };
				assert(fwrite(size_data, sizeof(u16), 2, compressed_data_file) == 2);
				u8 channels_image = static_cast<u8>(image.channels); 
				assert(fwrite(&channels_image, 1, 1, compressed_data_file) == 1);
				assert(fwrite(compressed_data.data(), 1, compressed_data.size(), compressed_data_file) == compressed_data.size());
				assert(fclose(compressed_data_file) == 0);
			}
		}
		textures_.push_back(impl);
	}
	
}

#ifdef GLTF_USE_MANY_BUFFERS
#define SetupAttribute(A,B,N0,N1,C,D,E,F) applyAccessorAsAttribute(A,B,C,D,E,F, views)
#else
#define SetupAttribute(A,B,D,E,F,G,H,I) applyAccessorAsAttributeSingleBufferUnskinned(A,B,D,E,F,G,H,I)
#endif

AABB Mesh::processPrimitiveAttribs(size_t &file_buffer_id, std::fstream &file, gltf::data &data, SharedPtr<VertexArray> const &vertex_array, gltf::primitive const &primitive,
		Vec<SharedPtr<Buffer>> &views) {
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
	std::optional<gltf::id> position_accessor = std::nullopt;
	std::optional<gltf::id> normal_accessor = std::nullopt;
	std::optional<gltf::id> uv_accessor = std::nullopt;
	
	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		gltfDebugPrintf("Accessor of name %s", name.c_str());
		switch (hash(name)) {
			case hash("POSITION"):
				position_accessor = accessor_id;
				gltfDebugPrint("POSITION attribute identified");
				SetupAttribute(file_buffer_id,file,vertex_buffer_,0,data,0,vertex_array,accessor);
				break;
			case hash("NORMAL"):
				normal_accessor = accessor_id;
				gltfDebugPrint("NORMAL attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 12, data, 1, vertex_array, accessor);
				break;
			case hash("TEXCOORD_0"):
				uv_accessor = accessor_id;
				gltfDebugPrint("TEXCOORD_0 attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 24, data, 3, vertex_array, accessor);
				break;
			case hash("TEXCOORD_1"):
				gltfDebugPrint("TEXCOORD_1 attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 48, data, 4, vertex_array, accessor);
				break;
			case hash("TANGENT"):
				gltfDebugPrint("TANGENT attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 32, data, 2, vertex_array, accessor);
				break;
			case hash("JOINTS_0"):
				gltfDebugPrint("JOINTS_0 attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 44, data, 4, vertex_array, accessor);
				break;
			case hash("WEIGHTS_0"):
				gltfDebugPrint("WEIGHTS_0 attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 48, data, 5, vertex_array, accessor);
				break;
			default: break;
		}
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

void Mesh::applyAccessorAsAttribute(size_t &file_buffer_id, std::fstream &file, gltf::data const &data, i32 const index, _STD shared_ptr<VertexArray> vertex_array, gltf::accessor const &accessor, Vec<SharedPtr<Buffer>> &views) {
#ifdef GLTF_USE_MANY_BUFFERS
	VertexAttribute_t attrib{};
	attrib.stride = 0;
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
		auto* t = reinterpret_cast<const float*>(&gltf_buffer.data()[buffer_view.offset]);
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
	
		buffer->upload(
			data_length, &gltf_buffer.data()[data_offset],
			gl::BufferUsageARB::StaticDraw
		);

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
	attrib.stride = static_cast<i32>(buffer_view.stride == 0 ? attrib.size * static_cast<i32>(size) : buffer_view.stride);
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
	VertexAttribute_t attrib{};
	attrib.offset = static_cast<gltf::id>(offset);
	attrib.type = gltf::gpuComponentTypeFromGltfComponentType(accessor.componentType());
	attrib.size = static_cast<gltf::id>(gltf::sizeForComponentType(accessor.componentType()));
	attrib.binding = 0;
	attrib.stride = 64;
	attrib.normalized = false;
	attrib.index = index;
	// delete[] raw_data;
	vertex_array->setAttribute(attrib);
	gpu_check;
}

void Mesh::applyAccessorAsElementBuffer(size_t &file_buffer_id, std::fstream &file, gltf::data const &data, _STD shared_ptr<VertexArray> vertex_array, gltf::accessor const &accessor,
		Vec<SharedPtr<Buffer>> &views) {
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	SharedPtr<Buffer> &buffer = views[accessor.bufferView()];
	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

	if (buffer.get() == nullptr) {
		buffer.reset(new Buffer());
		assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);
		
		size_t const data_offset = buffer_view.offset;
		size_t const data_length = buffer_view.length;
	
		buffer->upload(
			data_length, &gltf_buffer.data()[data_offset],
			gl::BufferUsageARB::StaticDraw
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
	
	gpu_check;
	
	gpu_check;

	
	vertex_array->bind();

	vertex_array->elements_count = accessor.count();
	vertex_array->offset_of_elements = accessor.offset(); //< offset is in bytes.
	
	vertex_array->draw_elements_type = gl::DrawElementsType::UnsignedShort;
	vertex_array->setElementBuffer(*buffer);
	gpu_check;

	gltfDebugPrintf("Element buffer applied with %llu elements", accessor.count());
}
