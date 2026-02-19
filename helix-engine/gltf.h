#pragma once
#include <array>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <cstdio>
#include <future>

#include "graphics-enums.hpp"

#define GLTF_DEBUG 1

#if GLTF_DEBUG == 1

namespace gl {
	enum class TextureMagFilter : gl::enum_t;
}
#define gltfDebugPrint(str) (printf("[%s:%d] %s\n", &std::string(__FILE__)[42], __LINE__, str))
#define gltfDebugPrintf(str, ...) (printf("[%s:%d] ", &std::string(__FILE__)[42], __LINE__), printf(str, __VA_ARGS__), printf("\n"))

#else

#define gltfDebugPrint(...)
#define gltfDebugPrintf(...)

#endif

#define GLTF_NUMBER float

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "types.hpp"
#include "glad/glad.h"
#include "simdjson/simdjson.h"

namespace gltf {
	#ifdef GLTF_NUMBER_IS_DOUBLE
	using number = std::double_t;
	#else
	using number = std::float_t;
	#endif

	using id = std::int32_t;
}

enum class GltfComponentType_e : std::uint8_t {
	BYTE,
	UBYTE,
	SHORT,
	USHORT,
	FLOAT
};
constexpr char const *to_string(GltfComponentType_e e) {
	switch (e) {
		case GltfComponentType_e::BYTE: return "BYTE";
		case GltfComponentType_e::UBYTE: return "UBYTE";
		case GltfComponentType_e::SHORT: return "SHORT";
		case GltfComponentType_e::USHORT: return "USHORT";
		case GltfComponentType_e::FLOAT: return "FLOAT";
	}
	return "unknown"; // <--- stupid shit to shut up the ide
}

constexpr static u32 componentTypeToGL(GltfComponentType_e e) {
	switch (e) {
		case GltfComponentType_e::BYTE: return GL_BYTE;
		case GltfComponentType_e::UBYTE: return GL_UNSIGNED_BYTE;
		case GltfComponentType_e::SHORT: return GL_SHORT;
		case GltfComponentType_e::USHORT: return GL_UNSIGNED_SHORT;
		case GltfComponentType_e::FLOAT: return GL_FLOAT;
	}
	return GL_NONE;
} 

enum class GltfType_e : std::uint8_t {
	SCALAR,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4,
};



constexpr char const *to_string(GltfType_e e) {
	switch (e) {
		case GltfType_e::SCALAR: return "SCALAR";
		case GltfType_e::VEC2: return "VEC2";
		case GltfType_e::VEC3: return "VEC3";
		case GltfType_e::VEC4: return "VEC4";
		case GltfType_e::MAT2: return "MAT2";
		case GltfType_e::MAT3: return "MAT3";
		case GltfType_e::MAT4: return "MAT4";
	}
	return "unknown"; // <--- stupid shit to shut up the ide
}

class CGltfProperty {
public:
	virtual ~CGltfProperty() = default;

	void setName(std::string const& p_name);
	[[nodiscard]] std::string const& name() const;

private:
	std::string name_;
};

/**
 * 
 */
class CGltfAccessor : public CGltfProperty {
public:

	CGltfAccessor();

	void setComponentType(GltfComponentType_e p_type);
	[[nodiscard]] GltfComponentType_e componentType() const;

	void setType(GltfType_e p_type);
	[[nodiscard]] GltfType_e type() const;
	
	void setBufferView(gltf::id p_bufferViewIndex);
	[[nodiscard]] gltf::id bufferView() const;

	void setOffset(std::uint32_t p_offset);
	[[nodiscard]] std::uint32_t offset() const;

	void setCount(std::uint32_t p_count);
	[[nodiscard]] std::uint32_t count() const;

	void setMax(std::array<GLTF_NUMBER, 16> const& p_max);
	void setMaxComponent(std::size_t p_index, GLTF_NUMBER p_value);
	[[nodiscard]] std::array<GLTF_NUMBER, 16> const& max() const;
	
	void setMin(std::array<GLTF_NUMBER, 16> const& p_min);
	void setMinComponent(std::size_t p_index, GLTF_NUMBER p_value);
	[[nodiscard]] std::array<GLTF_NUMBER, 16> const& min() const;
	
private:
	GltfComponentType_e component_type_ = GltfComponentType_e::BYTE;
	GltfType_e type_ = GltfType_e::SCALAR;
	std::array<GLTF_NUMBER, 16> max_;
	std::array<GLTF_NUMBER, 16> min_;
	gltf::id buffer_view_ = 0u;
	std::uint32_t offset_ = 0u;
	std::uint32_t count_ = 0u;
	bool normalized_ = false;
};

struct GltfAccessor_t {
	GltfComponentType_e component_type = GltfComponentType_e::BYTE;
	GltfType_e type = GltfType_e::SCALAR;
	std::array<GLTF_NUMBER, 16> max;
	std::array<GLTF_NUMBER, 16> min;
	gltf::id buffer_view = 0u;
	std::uint32_t offset = 0u;
	std::uint32_t count = 0u;
	bool normalized = false;
};

/**
 * Buf
 * @section 5.10
 */
class CGltfBuffer {
public:

	CGltfBuffer() = default;
	CGltfBuffer(std::string const& uri, std::string const& name);
	CGltfBuffer(std::vector<char> &&data);

	[[nodiscard]] constexpr char const& operator[](std::size_t const index) const {
		return data_[index];
	}

	[[nodiscard]] constexpr std::size_t length() const { return data_.size(); }

	inline std::vector<char> const& data() const { return data_; }
	
private:
	std::vector<char> data_;
	std::optional<std::string> uri_, name_;
};

/**
 * Buffer View
 * @section 5.11
 */
class CGltfBufferView {
public:

	enum class ETarget : std::uint8_t {
		ARRAY,
		ELEMENT
	};
	
	CGltfBufferView(
		std::shared_ptr<CGltfBuffer> const& buffer,
		std::uint32_t length = 0, std::uint32_t offset = 0,
		ETarget target = ETarget::ARRAY);
	~CGltfBufferView() = default;
	
	[[nodiscard]] constexpr std::uint8_t const& operator[](std::size_t const index) const {
		assert(index + offset_ <= buffer_->length());
		return buffer_->operator[](index + offset_);
	}

	[[nodiscard]] constexpr ETarget target() const { return target_; }
	[[nodiscard]] constexpr std::uint32_t offset() const { return offset_; }
	[[nodiscard]] constexpr std::uint32_t length() const { return length_; }

private:
	std::shared_ptr<CGltfBuffer> buffer_;
	std::uint32_t length_ = 0u, offset_ = 0u;
	ETarget target_ = ETarget::ARRAY;
};

enum class GltfBufferViewTarget_e : std::uint16_t {
	ARRAY = 34962,
	ELEMENT = 34963
};

// we dont need a fancy thing
struct GltfBufferView_t {
	gltf::id buffer = 0;
	std::uint32_t length = 0u, offset = 0u;
	GltfBufferViewTarget_e target = GltfBufferViewTarget_e::ARRAY;
};

struct GltfCameraOrthographic_t {
	GLTF_NUMBER
		x_magnification,
		y_magnification,
		z_near, z_far;
};

struct GltfCameraPerspective_t {
	GLTF_NUMBER
		aspect_ratio, y_fov,
		z_far, z_near;
};

struct GltfCamera_t {
	std::variant<
		GltfCameraOrthographic_t,
		GltfCameraPerspective_t
	> camera;

	[[nodiscard]] constexpr bool is_orthographic() const{
		return std::holds_alternative<GltfCameraOrthographic_t>(camera);
	}
	[[nodiscard]] constexpr bool is_perspective() const{
		return std::holds_alternative<GltfCameraPerspective_t>(camera);
	}
	[[nodiscard]] constexpr GltfCameraOrthographic_t const& orthographic() const {
		return std::get<GltfCameraOrthographic_t>(camera);
	}
	[[nodiscard]] constexpr GltfCameraPerspective_t const& perspective() const {
		return std::get<GltfCameraPerspective_t>(camera);
	}
};

#ifdef GLTF_THREADED_IMAGE_LOADING
extern std::vector<std::thread> gltf_worker_threads_;

struct GltfImage_t {
	std::string name;
	std::string mimeType;
	std::string uri; //< If this is empty, check bufferView!
	gltf::id bufferView; //< Ensure that URI is unused!
	std::future<std::shared_ptr<std::vector<unsigned char>>> external_data;
};
#else

struct GltfImage_t {
	std::string name;
	std::string mimeType;
	std::string uri; //< If this is empty, check bufferView!
	gltf::id bufferView; //< Ensure that URI is unused!
	std::vector<unsigned char> external_data;
	gltf::id channels; //< Not a part of the glTF spec, but is used to share the information from assembling buffers & images to the gpu alloc stage.
	glm::ivec2 size;
};
#endif

struct PbrMetallicRoughness_t {
	gltf::id base_color_texture;
	gltf::id metallic_roughness_texture;
};

struct GltfMaterial_t {
	std::string name;
	gltf::id emissive_texture;
	gltf::id normal_texture;
	gltf::id occlusion_texture;
	PbrMetallicRoughness_t pbr_metallic_roughness;
};

struct GltfNode_t {
	std::string name;
	//union {
		glm::mat4 matrix;
	///	struct {
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 scale;
	//	};
	//};
	std::vector<gltf::id> children;
	std::vector<GLTF_NUMBER> weights;
	gltf::id camera = -1;
	gltf::id skin = -1;
	gltf::id mesh = -1;
};

struct GltfScene_t {
	std::string name;
	std::vector<gltf::id> nodes;
};

struct GltfMeshPrimitiveAttrib_t {
	std::string name;
	gltf::id accessor;
};

using GltfMeshPrimitiveAttributes = std::vector<GltfMeshPrimitiveAttrib_t>;

enum class GltfMeshPrimitiveMode_e : std::uint8_t {
	POINTS = 0,
	LINES,
	LINE_LOOP,
	LINE_STRIP,
	TRIANGLES,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
};

struct GltfMeshPrimitive_t {
	GltfMeshPrimitiveAttributes attributes;
	i32 indices = -1;
	u32 material = 0;
	GltfMeshPrimitiveMode_e mode = GltfMeshPrimitiveMode_e::TRIANGLES;
	
};

using GltfMeshPrimitives = std::vector<GltfMeshPrimitive_t>;

struct GltfMesh_t {
	std::string name;
	GltfMeshPrimitives primitives;
	std::vector<GLTF_NUMBER> weights; //< MUST be same size as morph targets.
};

struct GltfTexture_t {
	gltf::id sampler;
	gltf::id source; //< Image
};

struct GltfSampler_t {
	gl::TextureMagFilter mag_filter = gl::TextureMagFilter::Linear;
	gl::TextureMinFilter min_filter = gl::TextureMinFilter::Linear;
	gl::TextureWrapMode wrap_s_mode = gl::TextureWrapMode::Clamp;
	gl::TextureWrapMode wrap_t_mode = gl::TextureWrapMode::Clamp;
};

using GltfBuffers = std::vector<CGltfBuffer>;
using GltfAccessors = std::vector<CGltfAccessor>;
using GltfBufferViews = std::vector<GltfBufferView_t>;
using GltfCameras = std::vector<GltfCamera_t>;
using GltfImages = std::vector<GltfImage_t>;
using GltfNodes = std::vector<GltfNode_t>;
using GltfMeshes = std::vector<GltfMesh_t>;
using GltfTextures = std::vector<GltfTexture_t>;
using GltfSamplers = std::vector<GltfSampler_t>;
using GltfScenes = std::vector<GltfScene_t>;
using GltfMaterials = std::vector<GltfMaterial_t>;

struct GltfData_t {
	std::filesystem::path	path;
	GltfBuffers				buffers;
	GltfBufferViews			buffer_views;
	GltfAccessors			accessors;
	GltfCameras				cameras;
	GltfImages				images;
	GltfNodes				nodes;
	GltfMeshes				meshes;
	GltfTextures			textures;
	GltfSamplers			samplers;
	GltfScenes				scenes;
	GltfMaterials			materials;
	gltf::id				scene;
};

struct GltfFile_t {
	std::fstream file;
};

namespace gltf {
	extern GltfData_t parse(std::string const& file_path, simdjson::padded_string &&file);
}