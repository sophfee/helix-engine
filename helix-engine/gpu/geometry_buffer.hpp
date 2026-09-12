#pragma once

#include "driver.hpp"
#include "math.hpp"
#include "types.hpp"
#include "engine/rid.hpp"

namespace vk {
	class CommandBuffer;
}

class GBuffer {
	struct Storage {
		RID depth;
		RID color;
		RID direct_light;
		RID normal;
		RID position;
		RID orm;
	};
	Storage storage[gfx::frames_in_flight];
	
public:
	GBuffer();
	GBuffer(ivec2 const &resolution);
	GBuffer(GBuffer const &) = delete;
	GBuffer(GBuffer &&) = delete;
	GBuffer& operator=(GBuffer const &) = delete;
	GBuffer& operator=(GBuffer &&) = delete;
	~GBuffer();

	void change_resolution(ivec2 resolution);
	RenderingDescriptor get_rendering_info(const IWindow *window, std::uint32_t frame_index) const;

	[[nodiscard]] RID get_direct_lighting_texture(std::uint32_t frame_index) const;
	[[nodiscard]] RID get_normal_texture(std::uint32_t frame_index) const;
	[[nodiscard]] RID get_position_texture(std::uint32_t frame_index) const;
	[[nodiscard]] RID get_orm_texture(std::uint32_t frame_index) const;
	[[nodiscard]] RID get_depth_texture(std::uint32_t frame_index) const;
};
