#pragma once
#include "../driver.hpp"
#include <concepts>

// initially i did this to fuck around with c++20 concepts, but frankly this quickly became too much work to be remotely considered worthwhile at this time.
// we have runtime polymorphism and that works fine, concept fails don't even really provide a descriptive reason for why they fail...

template <typename T>
concept Backend = requires(T backend, const gfx::BufferDescriptor &buffer_desc,
                                    const gfx::ImageDescriptor &image_desc,
                                    RID rid, RID cmd_rid, RID pipeline_rid, RID buffer_rid,
                                    gfx::ShaderStage stage, gfx::IndexType index_type,
                                    gfx::PipelineBindPoint bind_point,
                                    std::uint32_t offset, std::uint32_t size, const void* data, u64 u64_offset) {
	{ T() } -> std::same_as<T>;
	{ backend.backend() } -> std::same_as<RenderingApiBackend>;
	// { backend.buffer_create(buffer_desc) } -> std::same_as<RID>;
	// { backend.image_create(image_desc) } -> std::same_as<RID>;
	// { backend.push_constants(cmd_rid, pipeline_rid, TODO, data) } -> std::same_as<void>;
	// { backend.pipeline_bind(pipeline_rid, cmd_rid, bind_point) } -> std::same_as<void>;
};