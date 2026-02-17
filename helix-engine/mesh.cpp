#include "mesh.hpp"
#include "gltf.h"
#include "stb/stb_image.h"

#include <future>

CMesh::CMesh() {
}
CMesh::CMesh(GltfData_t &data) {
	gltfDebugPrintf("Loading CMesh! We have %llu mesh(es)", data.meshes.size());
	
	for (GltfMesh_t &mesh : data.meshes) {
		auto vertex_array = std::make_shared<CVertexArray>();// = vertex_arrays_.back();
		vertex_array->bind();
		// Primitives
		for (GltfMeshPrimitive_t const &primitive : mesh.primitives) {
			for (auto const &[name, accessor_id] : primitive.attributes) {
				assert(data.accessors.size() > static_cast<std::size_t>(accessor_id));
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
					default: break;
				}
			}

			if (primitive.indices != -1) {
				assert(data.accessors.size() > static_cast<std::size_t>(primitive.indices));
				CGltfAccessor &accessor = data.accessors[primitive.indices];
				applyAccessorAsElementBuffer(data, vertex_array, accessor);
			}
		}
		vertex_arrays_.emplace_back(vertex_array);
	}
	vertex_arrays_.back()->unbind();

	// finish handling those images, they've had time to actually load now :)
	for (auto &[sampler, source] : data.textures) {
		auto texture = std::make_shared<CTexture>(gl::TextureTarget::Texture2D);

		auto &image = data.images[source];
		auto &[mag_filter, min_filter, wrap_s_mode, wrap_t_mode] = data.samplers[sampler];

		texture->setIntParam(gl::GetTextureParameter::TextureWrapS, static_cast<i32>(wrap_s_mode));
		texture->setIntParam(gl::GetTextureParameter::TextureWrapT, static_cast<i32>(wrap_t_mode));
		texture->setIntParam(gl::GetTextureParameter::TextureMagFilter, static_cast<i32>(mag_filter));
		texture->setIntParam(gl::GetTextureParameter::TextureMinFilter, static_cast<i32>(min_filter));


		texture->setImage2D(
			image.external_data.data(),
			0,
			glm::ivec2(0,0),
			glm::ivec2(4096, 4096),
			gl::PixelFormat::Rgb,
			gl::PixelType::UnsignedByte
		);
		
	}
}

CMesh::~CMesh() {
}

std::size_t CMesh::subMeshCount() const {
	return vertex_arrays_.size();
}

void CMesh::drawSubMesh(std::size_t const submesh) const {
	vertex_arrays_[submesh]->bind();
	vertex_arrays_[submesh]->draw();
}
void CMesh::drawAllSubMeshes() const {
	for (auto &vertex_array_ : vertex_arrays_) {
		vertex_array_->bind();
		vertex_array_->draw();
	}
}

void CMesh::applyAccessorAsAttribute(GltfData_t const &data, i32 index, std::shared_ptr<CVertexArray> vertex_array, CGltfAccessor const &accessor) {
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
	auto const buffer = std::make_shared<CBuffer>();

	buffer->setData(
		buffer_view.length,
		&gltf_buffer[buffer_view.offset],
		gl::BufferUsageARB::DynamicDraw
	);

	vertex_array->bind();

	vertex_array->setVertexBuffer(
		index,
		*buffer,
		attrib.size * static_cast<i32>(size),
		0
	);

	gpuDebugf("Generic attribute created for vao, index %i size %llu len %u and stride is %d", index, size, buffer_view.length, attrib.size * static_cast<i32>(size));

	buffers_.push_back(buffer);
	attrib.index = index;
	attrib.binding = index; // its convenient ig
	vertex_array->setAttribute(attrib);
}

void CMesh::applyAccessorAsElementBuffer(GltfData_t const &data, std::shared_ptr<CVertexArray> vertex_array, CGltfAccessor const &accessor) {
	GltfBufferView_t const buffer_view = data.buffer_views[accessor.bufferView()];
	CGltfBuffer const& gltf_buffer = data.buffers[buffer_view.buffer];
	auto const buffer = std::make_shared<CBuffer>();

	buffer->setData(
		buffer_view.length,
		&gltf_buffer[buffer_view.offset],
		gl::BufferUsageARB::DynamicDraw
	);
	
	vertex_array->bind();

	vertex_array->elements_count = accessor.count();
	vertex_array->draw_elements_type = gl::DrawElementsType::UnsignedShort;
	vertex_array->setElementBuffer(
		*buffer
	);

	gltfDebugPrintf("Element buffer applied with %u elements", accessor.count());

	buffers_.push_back(buffer);
}
