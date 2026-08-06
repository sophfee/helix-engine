#pragma once

#include <thread>

#include <vulkan/vulkan.hpp>

#include "types.hpp"
#include "engine/rid.hpp"
#include "glfw/glfw3.h"

class IRenderer;
class Buffer;
class Texture;
class Camera3D;

using Format = vk::Format;

extern void initGraphics();
extern void terminateGraphics();

#define GPU_DEBUG 1

#if GPU_DEBUG == 1
#define gpuDebug(str) (printf("[%s:%d] %s\n", &_STD string(__FILE__)[42], __LINE__, str))
#define gpuDebugf(str, ...) (printf("[%s:%d] ", &_STD string(__FILE__)[42], __LINE__), printf(str, __VA_ARGS__), printf("\n"))
#else
#define gpuDebug(...)
#define gpuDebugf(...)
#endif

namespace gpu {
	extern bool check(char const *where, _STD size_t const line);
}

#define gpu_check

// Program

enum class EComponentType : _STD uint8_t {
	HALF_FLOAT,
	SINGLE_FLOAT,
	DOUBLE_FLOAT,
	FIXED_FLOAT,
	SIGNED_BYTE,
	UNSIGNED_BYTE,
	SIGNED_SHORT,
	UNSIGNED_SHORT,
	SIGNED_INT,
	UNSIGNED_INT,
	INT_2_10_10_10,
	UNSIGNED_INT_2_10_10_10,
	UNSIGNED_INT_10F_11F_11F,
};

enum class RenderPassType {
	Normal,
	Shadow,
	CullCompute
};


/**
 * @brief A set of bindings to tell the material where to send the material's information.
 *
 * To properly send a material's information to a shader, you must supply a bridge.
 * This object dictates what shader parameters correspond to what material properties.
 * This is because different shaders may have different parameter names for the same material property,
 * and some shaders may not even use certain material properties at all. The bridge also allows for more
 * efficient rendering by avoiding unnecessary shader parameter updates for properties that are not used by the shader
 */
struct MaterialBridge {
	i32 diffuse_texture_unit = -1; //< Location for the diffuse texture is not necessary, as the shader will bind the unit to the shader correctly. Use `diffuse_texture` if using ARB_bindless_textures.
	i32 diffuse_texture = -1; //< Uses ARB_bindless_textures
	i32 diffuse_texture_is_used = -1;
	i32 diffuse_color_modulation = -1;

	i32 orm_texture_unit = -1;
	i32 orm_texture = -1;
	i32 orm_texture_is_used = -1;

	i32 occlusion_is_used = -1;
	i32 occlusion_strength = -1;

	i32 roughness_bias = -1; //< Bias = Offset
	i32 roughness_scale = -1; //< Scale multiplies the roughness channel of the ORM texture.

	i32 metallic_bias = -1; //< Bias = Offset
	i32 metallic_scale = -1; //< Scale multiplies the metallic channel of the ORM texture.

	i32 normal_texture_unit = -1;
	i32 normal_texture = -1;
	i32 normal_texture_is_used = -1;
	i32 normal_texture_strength = -1;

	i32 emissive_texture_unit = -1;
	i32 emissive_texture = -1;
	i32 emissive_texture_is_used = -1;

	i32 emissive_color_modulation = -1; //< Modulates the emissive color channel of the shader, allowing for dynamic changes to the emissive color without needing to update the texture.
	i32 emissive_blend_mode = -1; //< Generally: 0: Emission = (Texture * Modulation), 1: Emission = (Texture + Modulation)

	i32 emissive_bias = -1; //<
	i32 emissive_scale = -1;
};
struct RenderPassInfo {
	RenderPassType pass;
	vk::Device device;
	RID material_bind_group_layout;
	RID pipeline_layout;
	RID pipeline;
	RID cmd;
};