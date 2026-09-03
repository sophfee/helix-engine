#pragma once

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
		RID normal;
		RID position;
		RID orm;
		RID id;
		RID emissive;
	};
	Box<Storage> storage;
	
public:
	GBuffer();
	GBuffer(ivec2 const &resolution);
	GBuffer(GBuffer const &) = delete;
	GBuffer(GBuffer &&) = delete;
	GBuffer& operator=(GBuffer const &) = delete;
	GBuffer& operator=(GBuffer &&) = delete;
	~GBuffer();

	void change_resolution(ivec2 resolution);

	void begin_rendering(vk::CommandBuffer cmd) const;
	void end_rendering(vk::CommandBuffer cmd) const;

	[[nodiscard]] RID color() const;
	[[nodiscard]] RID normal() const;
	[[nodiscard]] RID position() const;
	[[nodiscard]] RID orm() const;
	[[nodiscard]] RID id() const;
	[[nodiscard]] RID emissive() const;
};
