#include "mesh.hpp"
#include "gltf.h"
#include "stb/stb_image.h"

#include <future>
#include <cassert>

CSkin::CSkin() {
}

CSkin::~CSkin() {
}

CMeshResource::CMeshResource() {
}

CMeshResource::CMeshResource(GltfData_t &data) : material_info_(data.materials) {
	for (GltfMesh_t &mesh : data.meshes)
		processMesh(data, mesh);
	primitives_.back().vertex_array->unbind();
	processTextures(data);
}

CMeshResource::CMeshResource(GltfData_t &data, _STD size_t const mesh_id) : is_skinned_(false), material_info_(data.materials) {
	processMesh(data, data.meshes[mesh_id]);
	processTextures(data);
}

CMeshResource::CMeshResource(GltfData_t &data, _STD size_t const mesh_id, [[maybe_unused]] _STD size_t skin_id) : is_skinned_(true), material_info_(data.materials) {
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
			base_color_texture = material_info_[material].pbr_metallic_roughness.base_color_texture,
			metallic_roughness_texture = material_info_[material].pbr_metallic_roughness.metallic_roughness_texture;
		
		if (base_color_texture > textures_.size())
			textures_[base_color_texture]->bindTextureUnit(0);
		
		if (metallic_roughness_texture > textures_.size())
			textures_[metallic_roughness_texture]->bindTextureUnit(1);
		
		vertex_array->bind();
		vertex_array->draw();
		gpu_check;
	}
}

bool CMeshResource::skinned() const {
	return skin_.has_value();
}

void CMeshResource::processMesh(GltfData_t &data, GltfMesh_t &mesh) {
	char i = '0';
	for (GltfMeshPrimitive_t const &primitive : mesh.primitives) {
		auto const vertex_array = _STD make_shared<CVertexArray>();// = primitives_.back();
		vertex_array->bind();
		_STD string name = mesh.name + "#" + i;
		vertex_array->setLabel(name);
		
		processPrimitiveAttribs(data, vertex_array, primitive);

		if (primitive.indices != -1) {
			assert(data.accessors.size() > static_cast<_STD size_t>(primitive.indices));
			CGltfAccessor &accessor = data.accessors[primitive.indices];
			applyAccessorAsElementBuffer(data, vertex_array, accessor);
		}

		u32 const material_value = primitive.material;
		
		primitives_.push_back({
			.vertex_array = vertex_array,
			.material = material_value,
		});
		i++;
	}
}

void CMeshResource::processMeshAndSkin(GltfData_t &data, GltfMesh_t &mesh, GltfSkin_t &skin) {
	processMesh(data, mesh);
	auto ssbo_inv_bind_matrices = _STD make_shared<CBuffer>();
	//ssbo_inv_bind_matrices->allocStorage(skin)
}
void CMeshResource::processTextures(GltfData_t &data) {
	// finish handling those images, they've had time to actually load now :)
	for (auto &[sampler, source] : data.textures) {
		auto texture = _STD make_shared<CTexture>(gl::TextureTarget::Texture2D);

		auto &image = data.images[source];
		auto &[mag_filter, min_filter, wrap_s_mode, wrap_t_mode] = data.samplers[sampler];

		texture->setIntParam(gl::GetTextureParameter::TextureWrapS, static_cast<i32>(wrap_s_mode));
		texture->setIntParam(gl::GetTextureParameter::TextureWrapT, static_cast<i32>(wrap_t_mode));
		texture->setIntParam(gl::GetTextureParameter::TextureMagFilter, static_cast<i32>(mag_filter));
		texture->setIntParam(gl::GetTextureParameter::TextureMinFilter, static_cast<i32>(min_filter));
		texture->allocate(image.size, 1, gl::InternalFormat::CompressedRgbS3tcDxt1Ext);
		texture->setLabel(image.name);

		gpu_check;
		texture->setImage2D(
			image.external_data.data(),
			0,
			glm::ivec2(0,0),
			image.size,
			gl::PixelFormat::Rgb,
			gl::PixelType::UnsignedByte
		);
		gpu_check;
		
		textures_.push_back(texture);
		image.external_data.clear();
	}
}

void CMeshResource::processPrimitiveAttribs(GltfData_t &data, CSharedPtr<CVertexArray> const &vertex_array, GltfMeshPrimitive_t const &primitive) {
	for (auto const &[name, accessor_id] : primitive.attributes) {
		assert(data.accessors.size() > static_cast<_STD size_t>(accessor_id));
		CGltfAccessor &accessor = data.accessors[accessor_id];

		gltfDebugPrintf("Accessor of name %s", name.c_str());
		switch (hash(name)) {
			case hash("POSITION"):
				gltfDebugPrint("POSITION attribute identified");
				applyAccessorAsAttribute(data, 0, vertex_array, accessor);
				break;
			case hash("NORMAL"):
				gltfDebugPrint("NORMAL attribute identified");
				applyAccessorAsAttribute(data, 1, vertex_array, accessor);
				break;
			case hash("TEXCOORD_0"):
				gltfDebugPrint("TEXCOORD_0 attribute identified");
				applyAccessorAsAttribute(data, 2, vertex_array, accessor);
				break;
			case hash("JOINTS_0"):
				gltfDebugPrint("JOINTS_0 attribute identified");
				applyAccessorAsAttribute(data, 3, vertex_array, accessor);
				break;
			case hash("WEIGHTS_0"):
				gltfDebugPrint("WEIGHTS_0 attribute identified");
				applyAccessorAsAttribute(data, 4, vertex_array, accessor);
				break;
			default: break;
		}
	}
}

void CMeshResource::applyAccessorAsAttribute(GltfData_t const &data, i32 index, _STD shared_ptr<CVertexArray> vertex_array, CGltfAccessor const &accessor) {
	VertexAttribute_t attrib{};
	attrib.stride = 0;
	attrib.offset = 0;

	switch (accessor.type()) {
		case GltfType_e::SCALAR:	attrib.size = 1; 	break;
		case GltfType_e::VEC2:		attrib.size = 2; 	break;
		case GltfType_e::VEC3:		attrib.size = 3; 	break;
		case GltfType_e::VEC4:		attrib.size = 4;	break;  // NOLINT(bugprone-branch-clone)
		case GltfType_e::MAT2:		attrib.size = 4;	break;
		case GltfType_e::MAT3:		attrib.size = 9;	break;
		case GltfType_e::MAT4:		attrib.size = 16;	break;
	}

	size_t size = 0;
	switch (accessor.componentType()) {
		case GltfComponentType_e::BYTE:		attrib.type = EComponentType::SIGNED_BYTE;		size = sizeof(i8);				break;
		case GltfComponentType_e::UBYTE:	attrib.type = EComponentType::UNSIGNED_BYTE;	size = sizeof(u8);				break;
		case GltfComponentType_e::SHORT:	attrib.type = EComponentType::SIGNED_SHORT;		size = sizeof(i16);				break;
		case GltfComponentType_e::USHORT:	attrib.type = EComponentType::UNSIGNED_SHORT;	size = sizeof(u16);				break;
		case GltfComponentType_e::FLOAT:	attrib.type = EComponentType::SINGLE_FLOAT;		size = sizeof(gltf::number);	break;
	}
	
	GltfBufferView_t const buffer_view = data.buffer_views[accessor.bufferView()];
	CGltfBuffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	auto const buffer = _STD make_shared<CBuffer>();

	buffer->setData(
		buffer_view.length,
		&gltf_buffer[buffer_view.offset],
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
}

void CMeshResource::applyAccessorAsElementBuffer(GltfData_t const &data, _STD shared_ptr<CVertexArray> vertex_array, CGltfAccessor const &accessor) {
	GltfBufferView_t const buffer_view = data.buffer_views[accessor.bufferView()];
	CGltfBuffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	auto const buffer = _STD make_shared<CBuffer>();
	gpu_check;

	assert(gltf_buffer.length() >= buffer_view.offset + buffer_view.length);

	buffer->setData(
		buffer_view.length,
		&gltf_buffer[buffer_view.offset],
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
