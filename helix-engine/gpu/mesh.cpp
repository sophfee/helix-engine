#include "mesh.hpp"
#include "gltf.h"
#include "stb/stb_image.h"

#include <future>
#include <cassert>
#include <utility>

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

CMeshResource::CMeshResource() {
}

CMeshResource::CMeshResource(gltf::data &data) : material_info_(data.materials) {
	for (gltf::mesh &mesh : data.meshes)
		processMesh(data, mesh);
	primitives_.back().vertex_array->unbind();
	processTextures(data);
}

CMeshResource::CMeshResource(gltf::data &data, _STD size_t const mesh_id) : is_skinned_(false), material_info_(data.materials) {
	processMesh(data, data.meshes[mesh_id]);
	processTextures(data);
}

CMeshResource::CMeshResource(gltf::data &data, _STD size_t const mesh_id, [[maybe_unused]] _STD size_t skin_id) : is_skinned_(true), material_info_(data.materials) {
	processMesh(data, data.meshes[mesh_id]);
	processTextures(data);
}

CMeshResource::~CMeshResource() {
}

_STD size_t CMeshResource::subMeshCount() const {
	return primitives_.size();
}

void CMeshResource::drawSubMesh(_STD size_t const submesh) const {
	gpu_check;
	primitives_[submesh].vertex_array->bind();
	primitives_[submesh].vertex_array->draw();
	gpu_check;
}
void CMeshResource::drawAllSubMeshes() const {
	for (auto const &[vertex_array, material] : primitives_) {
		_STD size_t const
			base_color_texture = material_info_[material].pbr_metallic_roughness.base_color_texture.index,
			metallic_roughness_texture = material_info_[material].pbr_metallic_roughness.metallic_roughness_texture.index;

		#if 0
		if (base_color_texture > textures_.size())
			textures_[base_color_texture]->bindTextureUnit(0);
		
		if (metallic_roughness_texture > textures_.size())
			textures_[metallic_roughness_texture]->bindTextureUnit(1);
		#endif 
		vertex_array->bind();
		vertex_array->draw();
		gpu_check;
	}
}

bool CMeshResource::skinned() const {
	return skin_.has_value();
}

namespace {
	std::vector<char> temp_alloc_buffer_(0);
}
constexpr auto alloc_block_step = 0x100000;
void CMeshResource::processMesh(gltf::data &data, gltf::mesh const &mesh) {
	char i = '0';
	std::fstream file;
	size_t file_buffer_id = UINT64_MAX;
	for (gltf::primitive const &primitive : mesh.primitives) {
		auto const vertex_array = _STD make_shared<CVertexArray>();// = primitives_.back();
		vertex_array->bind();
		_STD string name = mesh.name + "#" + i;
		vertex_array->setLabel(name);
		
		processPrimitiveAttribs(file_buffer_id, file, data, vertex_array, primitive);

		if (primitive.indices != -1) {
			assert(data.accessors.size() > static_cast<_STD size_t>(primitive.indices));
			gltf::accessor &accessor = data.accessors[primitive.indices];
			applyAccessorAsElementBuffer(file_buffer_id, file, data, vertex_array, accessor);
		}

		u32 const material_value = primitive.material;
		
		primitives_.push_back({
			.vertex_array = vertex_array,
			.material = material_value,
		});
		i++;
	}
}

void CMeshResource::processMeshAndSkin(gltf::data &data, gltf::mesh &mesh, gltf::skin &skin) {
	processMesh(data, mesh);
	auto ssbo_inv_bind_matrices = _STD make_shared<CBuffer>();
	//ssbo_inv_bind_matrices->allocStorage(skin)
}
void CMeshResource::processTextures(gltf::data &data) {
	// finish handling those images, they've had time to actually load now :)
#if 0
	for (auto &[sampler, source] : data.textures) {
		auto texture = _STD make_shared<CTexture>(gl::TextureTarget::Texture2D);

		auto &image = data.images[source];
		auto &[mag_filter, min_filter, wrap_s_mode, wrap_t_mode] = data.samplers[sampler];

		//texture->allocate(image.size, 1, gl::InternalFormat::Rgba8);
		texture->setLabel(image.name);
		texture->setImage2D(
			image.external_data.data(),
			0,
			glm::ivec2(0,0),
			image.size,
			gl::PixelFormat::Rgba,
			gl::PixelType::UnsignedByte
		);
		textures_.push_back(texture);
		image.external_data.clear();
	}
#endif
}

#ifdef GLTF_USE_MANY_BUFFERS
#define SetupAttribute(A,B,N0,N1,C,D,E,F) applyAccessorAsAttribute(A,B,C,D,E,F)
#else
#define SetupAttribute(A,B,D,E,F,G,H,I) applyAccessorAsAttributeSingleBuffer(A,B,D,E,F,G,H,I)
#endif

void CMeshResource::processPrimitiveAttribs(size_t &file_buffer_id, std::fstream &file, gltf::data &data, CSharedPtr<CVertexArray> const &vertex_array, gltf::primitive const &primitive) {
	_STD vector<skinned_vertex> vertex_buffer_;
	//_STD size_t vertex_size_ = 0;
	_STD size_t count_ = 0;

	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		//vertex_size_ += gltf::componentsForType(accessor.type()) * gltf::sizeForComponentType(accessor.componentType());
		count_ = std::max(count_, accessor.count());
	}
#ifndef GLTF_USE_MANY_BUFFERS
	auto const buf = _STD make_shared<CBuffer>();
#endif
	vertex_buffer_.resize(count_);
	
	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		gltf::accessor &accessor = data.accessors[accessor_id];
		gltfDebugPrintf("Accessor of name %s", name.c_str());
		switch (hash(name)) {
			case hash("POSITION"):
				gltfDebugPrint("POSITION attribute identified");
				applyAccessorAsAttributeSingleBuffer(file_buffer_id,file,vertex_buffer_,0,data,0,vertex_array,accessor);
				break;
			case hash("NORMAL"):
				gltfDebugPrint("NORMAL attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 12, data, 1, vertex_array, accessor);
				break;
			case hash("TEXCOORD_0"):
				gltfDebugPrint("TEXCOORD_0 attribute identified");
				SetupAttribute(file_buffer_id, file, vertex_buffer_, 36, data, 3, vertex_array, accessor);
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

	buf->setData(
		sizeof(skinned_vertex) * vertex_buffer_.size(),
		vertex_buffer_.data(),
		gl::BufferUsageARB::DynamicDraw
	);
	gpu_check;
	
	buffers_.push_back(buf);
	vertex_array->setVertexBuffer(0, *buf, 64, 0);
}

void CMeshResource::applyAccessorAsAttribute(size_t &file_buffer_id, std::fstream &file, gltf::data const &data, i32 index, _STD shared_ptr<CVertexArray> vertex_array, gltf::accessor const &accessor) {
#ifdef GLTF_USE_MANY_BUFFERS
	VertexAttribute_t attrib{};
	attrib.stride = 0;
	attrib.offset = 0;

	attrib.size = gltf::componentsForType(accessor.type());

	size_t const size = gltf::sizeForComponentType(accessor.componentType());
	attrib.type = gltf::gpuComponentTypeFromGltfComponentType(accessor.componentType());
	
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	auto const buffer = _STD make_shared<CBuffer>();

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

void CMeshResource::applyAccessorAsAttributeSingleBuffer(
	size_t &file_buffer_id,
	std::fstream &file,
	std::vector<skinned_vertex> &buffer,
	size_t const offset,
	gltf::data const &data,
	i32 const index,
	CSharedPtr<CVertexArray> const &vertex_array,
	gltf::accessor const &accessor
) {
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];

	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

	if (std::cmp_not_equal(buffer_view.buffer, file_buffer_id)) {
		file = std::fstream(gltf_buffer.uri(),
			std::ios::binary);
		file_buffer_id = buffer_view.buffer;
	}

	u64 const attribute_element_size = gltf::sizeForComponentType(accessor.componentType()) * gltf::componentsForType(accessor.type());
	u64 const attribute_buffer_size = buffer_view.length;
	auto const raw_data = new char[attribute_buffer_size];
	
	file.seekg(static_cast<std::fstream::off_type>(buffer_view.offset), std::ios::beg);
	file.read(raw_data, static_cast<std::streamsize>(attribute_buffer_size));
	
	for (size_t i = 0; i < accessor.count(); ++i)
		*(reinterpret_cast<u8 *>(&buffer[i]) + offset) = *(raw_data + attribute_element_size * i);

	VertexAttribute_t attrib{};
	attrib.binding = 0;
	attrib.offset = static_cast<gltf::id>(offset);
	attrib.type = gltf::gpuComponentTypeFromGltfComponentType(accessor.componentType());
	attrib.size = static_cast<gltf::id>(gltf::sizeForComponentType(accessor.componentType()));
	attrib.binding = 0;
	attrib.stride = 64;
	attrib.normalized = false;
	delete[] raw_data;
	vertex_array->setAttribute(attrib);
	gpu_check;
}

void CMeshResource::applyAccessorAsElementBuffer(size_t const &file_buffer_id, std::fstream &file, gltf::data const &data, _STD shared_ptr<CVertexArray> vertex_array, gltf::accessor const &accessor) {
	gltf::buffer_view const buffer_view = data.buffer_views[accessor.bufferView()];
	gltf::buffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	auto const buffer = _STD make_shared<CBuffer>();
	gpu_check;

	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

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

	vertex_array->elements_count = accessor.count();
	vertex_array->draw_elements_type = gl::DrawElementsType::UnsignedShort;
	vertex_array->setElementBuffer(
		*buffer
	);
	gpu_check;

	gltfDebugPrintf("Element buffer applied with %llu elements", accessor.count());

	buffers_.push_back(buffer);
}
