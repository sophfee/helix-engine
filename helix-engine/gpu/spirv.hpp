#pragma once

#include <fstream>

#include "driver.hpp"
#include "types.hpp"

namespace spirv {
	static RID load(const Path &path) {
		std::ifstream stream(path, std::ios::binary | std::ios::ate);
		std::vector<char> code(stream.tellg());
		stream.seekg(0, std::ios::beg);
		stream.read(code.data(), static_cast<std::streamsize>(code.size()));
		const SpirvDescriptor spirv_desc{
			.label = path.filename().string(),
			.code_size = code.size(),
			.code = reinterpret_cast<const uint32_t *>(code.data())
		};
		return GraphicsSystem::get_driver()->create_shader(spirv_desc);
	}
}
