#include <vulkan/vulkan.hpp>
#include "geometry_buffer.hpp"

GBuffer::GBuffer() : storage(nullptr) {
}

GBuffer::GBuffer(ivec2 const &resolution) {
	changeResolution(resolution);
}

GBuffer::~GBuffer() = default;

void GBuffer::changeResolution(ivec2 resolution) {
}

void GBuffer::beginRendering(vk::CommandBuffer cmd) const {
	// TODO
}

void GBuffer::endRendering(vk::CommandBuffer cmd) const {
	// TODO
}

RID GBuffer::color() const {
	return storage->color;
}
RID GBuffer::normal() const {
	return storage->normal;
}
RID GBuffer::position() const {
	return storage->position;
}
RID GBuffer::orm() const {
	return storage->orm;
}
RID GBuffer::id() const {
	return storage->id;
}
RID GBuffer::emissive() const {
	return storage->emissive;
}