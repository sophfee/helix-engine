#include <vulkan/vulkan.hpp>
#include "geometry_buffer.hpp"

GBuffer::GBuffer() : storage(nullptr) {
}

GBuffer::GBuffer(ivec2 const &resolution) {
	change_resolution(resolution);
}

GBuffer::~GBuffer() = default;

void GBuffer::change_resolution(ivec2 resolution) {
}

void GBuffer::begin_rendering(vk::CommandBuffer cmd) const {
	// TODO
}

void GBuffer::end_rendering(vk::CommandBuffer cmd) const {
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