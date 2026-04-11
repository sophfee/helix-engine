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
	for (gltf::mesh &mesh : data.meshes)
		processMesh(data, mesh);
	primitives_.back().vertex_array->unbind();
	processTextures(data);
}

Mesh::Mesh(gltf::data &data, _STD size_t const mesh_id) : is_skinned_(false), material_info_(data.materials) {
	processMesh(data, data.meshes[mesh_id]);
	processTextures(data);
}

Mesh::Mesh(gltf::data &data, _STD size_t const mesh_id, [[maybe_unused]] _STD size_t skin_id) : is_skinned_(true), material_info_(data.materials) {
	processMesh(data, data.meshes[mesh_id]);
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
				goto apply_normal_texture;;
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
	for (auto const &[vertex_array, aabb, material] : primitives_) {
		_STD size_t const
			base_color_texture = material_info_[material].pbr_metallic_roughness.base_color_texture.index,
			metallic_roughness_texture = material_info_[material].pbr_metallic_roughness.metallic_roughness_texture.index,
			normal_texture = material_info_[material].normal_texture.index;
		
		if (base_color_texture < textures_.size() && info.bind_albedo_texture) {
			if (textures_[base_color_texture] != nullptr) {
				if (textures_[base_color_texture]->isValid()) {
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
		vertex_array->draw();
		gpu_check;
	}
}

bool Mesh::skinned() const {
	return skin_.has_value();
}

namespace {
	std::vector<char> temp_alloc_buffer_(0);
}
constexpr auto alloc_block_step = 0x100000;
void Mesh::processMesh(gltf::data &data, gltf::mesh const &mesh) {
	char i = '0';
	std::fstream file(data.buffers[0].uri(), std::ios::binary);
	size_t file_buffer_id = 0ull;
	
	for (gltf::primitive const &primitive : mesh.primitives) {
		auto const vertex_array = _STD make_shared<VertexArray>();// = primitives_.back();
		vertex_array->bind();
		_STD string name = mesh.name + "#" + i;
		vertex_array->setLabel(name);
		
		AABB const aabb = processPrimitiveAttribs(file_buffer_id, file, data, vertex_array, primitive);

		if (primitive.indices != -1) {
			assert(data.accessors.size() > static_cast<_STD size_t>(primitive.indices));
			gltf::accessor &accessor = data.accessors[primitive.indices];
			applyAccessorAsElementBuffer(file_buffer_id, file, data, vertex_array, accessor);
		}

		u32 const material_value = primitive.material;
		
		primitives_.push_back({
			.vertex_array = vertex_array,
			.aabb_ = aabb,
			.material = material_value,
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
	processMesh(data, mesh);
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
			Error const res = uploadDdsFromStdio(F, impl->texture_object_);
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
#define SetupAttribute(A,B,N0,N1,C,D,E,F) applyAccessorAsAttribute(A,B,C,D,E,F)
#else
#define SetupAttribute(A,B,D,E,F,G,H,I) applyAccessorAsAttributeSingleBufferUnskinned(A,B,D,E,F,G,H,I)
#endif

AABB Mesh::processPrimitiveAttribs(size_t &file_buffer_id, std::fstream &file, gltf::data &data, SharedPtr<VertexArray> const &vertex_array, gltf::primitive const &primitive) {
	_STD vector<StandardVertex> vertex_buffer_;
	//_STD size_t vertex_size_ = 0;
	_STD size_t count_ = 0;

	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		//vertex_size_ += gltf::componentsForType(accessor.type()) * gltf::sizeForComponentType(accessor.componentType());
		count_ = std::max(count_, accessor.count());
	}
#ifndef GLTF_USE_MANY_BUFFERS
	auto const buf = _STD make_shared<Buffer>();
#endif
	vertex_buffer_.resize(count_);
	
	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		gltfDebugPrintf("Accessor of name %s", name.c_str());
		switch (hash(name)) {
			case hash("POSITION"):
				gltfDebugPrint("POSITION attribute identified");
				SetupAttribute(file_buffer_id,file,vertex_buffer_,0,data,0,vertex_array,accessor);
				break;
			case hash("NORMAL"):
				gltfDebugPrint("NORMAL attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 12, data, 1, vertex_array, accessor);
				break;
			case hash("TEXCOORD_0"):
				gltfDebugPrint("TEXCOORD_0 attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 24, data, 2, vertex_array, accessor);
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

	buf->upload(
		sizeof(StandardVertex) * vertex_buffer_.size(),
		vertex_buffer_.data(),
		gl::BufferUsageARB::StaticDraw
	);
	gpu_check;
	
	buffers_.push_back(buf);
	vertex_array->setVertexBuffer(0, *buf, 32, 0);

	return processAABB(vertex_buffer_);
}

void Mesh::applyAccessorAsAttribute(size_t &file_buffer_id, std::fstream &file, gltf::data const &data, i32 index, _STD shared_ptr<VertexArray> vertex_array, gltf::accessor const &accessor) {
#ifdef GLTF_USE_MANY_BUFFERS
	VertexAttribute_t attrib{};
	attrib.stride = 0;
	attrib.offset = 0;

	attrib.size = gltf::componentsForType(accessor.type());

	size_t const size = gltf::sizeForComponentType(accessor.componentType());
	attrib.type = gltf::gpuComponentTypeFromGltfComponentType(accessor.componentType());
	
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	auto const buffer = _STD make_shared<Buffer>();

	if (std::cmp_not_equal(buffer_view.buffer, file_buffer_id))
		file = std::fstream(gltf_buffer.uri(),
			std::ios::binary);
	
	_STD fstream::off_type const offset = static_cast<_STD fstream::off_type>(buffer_view.offset);
	_STD streamsize const length = static_cast<_STD streamsize>(buffer_view.length);

	if (std::cmp_less_equal(length, temp_alloc_buffer_.size()))
		temp_alloc_buffer_.resize(
			alloc_block_step * (
				1 + (length - length % alloc_block_step)
				  / alloc_block_step));

	file.seekg(offset, std::ios::beg);
	file.read(temp_alloc_buffer_.data(), length);

	buffer->setData(
		buffer_view.length,
		temp_alloc_buffer_.data(),
		gl::BufferUsageARB::DynamicDraw
	);
	gpu_check;

	vertex_array->bind();

	vertex_array->setVertexBuffer(
		index,
		*buffer,
		attrib.size * static_cast<i32>(size),
		0
	);
	gpu_check;

	gpuDebugf("Generic attribute created for vao, index %i size %llu len %u and stride is %d", index, size, buffer_view.length, attrib.size * static_cast<i32>(size));

	buffers_.push_back(buffer);
	attrib.index = index;
	attrib.binding = index; // its convenient ig
	attrib.stride = attrib.size * size;
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

void Mesh::applyAccessorAsElementBuffer(size_t &file_buffer_id, std::fstream &file, gltf::data const &data, _STD shared_ptr<VertexArray> vertex_array, gltf::accessor const &accessor) {
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	auto const buffer = _STD make_shared<Buffer>();
	gpu_check;

	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

	_STD fstream::off_type const offset = static_cast<_STD fstream::off_type>(buffer_view.offset);
	_STD streamsize const length = static_cast<_STD streamsize>(buffer_view.length);
	
	buffer->upload(
		length, &gltf_buffer.data()[offset],
		gl::BufferUsageARB::DynamicDraw
	);
	
	gpu_check;
	
	vertex_array->bind();

	vertex_array->elements_count = accessor.count();
	vertex_array->draw_elements_type = gl::DrawElementsType::UnsignedShort;
	vertex_array->setElementBuffer(
		*buffer
	);
	gpu_check;

	gltfDebugPrintf("Element buffer applied with %llu elements", accessor.count());

	buffers_.push_back(buffer);
}
