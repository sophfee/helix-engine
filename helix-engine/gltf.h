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
#define gltfDebugPrint(str) (printf("[%s:%d] %s\n", &_STD string(__FILE__)[42], __LINE__, str))
#define gltfDebugPrintf(str, ...) (printf("[%s:%d] ", &_STD string(__FILE__)[42], __LINE__), printf(str, __VA_ARGS__), printf("\n"))

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
	using number = _STD double_t;
	#else
	using number = _STD float_t;
	#endif

	using id = _STD int32_t;
	using size = _STD size_t;
}

enum class GltfComponentType_e : _STD uint8_t {
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

enum class GltfType_e : _STD uint8_t {
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

	void setName(_STD string const& p_name);
	[[nodiscard]] _STD string const& name() const;

private:
	_STD string name_;
};

/**
 * 
 */
class CGltfAccessor : public CGltfProperty {
public:

	CGltfAccessor();
	~CGltfAccessor();

	void setComponentType(GltfComponentType_e p_type);
	[[nodiscard]] GltfComponentType_e componentType() const;

	void setType(GltfType_e p_type);
	[[nodiscard]] GltfType_e type() const;
	
	void setBufferView(gltf::id p_bufferViewIndex);
	[[nodiscard]] gltf::id bufferView() const;

	void setOffset(gltf::size p_offset);
	[[nodiscard]]  gltf::size offset() const;

	void setCount(gltf::size p_count);
	[[nodiscard]] gltf::size count() const;

	void setMax(_STD array<GLTF_NUMBER, 16> const& p_max);
	void setMaxComponent(_STD size_t p_index, GLTF_NUMBER p_value);
	[[nodiscard]] _STD array<GLTF_NUMBER, 16> const& max() const;
	
	void setMin(_STD array<GLTF_NUMBER, 16> const& p_min);
	void setMinComponent(_STD size_t p_index, GLTF_NUMBER p_value);
	[[nodiscard]] _STD array<GLTF_NUMBER, 16> const& min() const;
	
private:
	GltfComponentType_e component_type_ = GltfComponentType_e::BYTE;
	GltfType_e type_ = GltfType_e::SCALAR;
	_STD array<GLTF_NUMBER, 16> max_;
	_STD array<GLTF_NUMBER, 16> min_;
	gltf::id buffer_view_ = 0u;
	gltf::size offset_ = 0u;
	gltf::size count_ = 0u;
	bool normalized_ = false;
};

struct GltfAccessor_t {
	GltfComponentType_e component_type = GltfComponentType_e::BYTE;
	GltfType_e type = GltfType_e::SCALAR;
	_STD array<GLTF_NUMBER, 16> max;
	_STD array<GLTF_NUMBER, 16> min;
	gltf::id buffer_view = 0u;
	gltf::size offset = 0u;
	gltf::size count = 0u;
	bool normalized = false;
};

/**
 * Buf
 * @section 5.10
 */
class CGltfBuffer {
public:

	CGltfBuffer() = default;
	CGltfBuffer(_STD string const& uri, _STD string const& name);
	CGltfBuffer(_STD vector<char> &&data);

	[[nodiscard]] constexpr char const& operator[](_STD size_t const index) const {
		return data_[index];
	}

	[[nodiscard]] constexpr gltf::size length() const { return data_.size(); }

	inline _STD vector<char> const& data() const { return data_; }
	
private:
	_STD vector<char> data_;
	_STD optional<_STD string> uri_, name_;
};

/**
 * Buffer View
 * @section 5.11
 */
class CGltfBufferView {
public:

	enum class ETarget : _STD uint8_t {
		ARRAY,
		ELEMENT
	};
	
	CGltfBufferView(
		_STD shared_ptr<CGltfBuffer> const& buffer,
		_STD uint32_t length = 0, _STD uint32_t offset = 0,
		ETarget target = ETarget::ARRAY);
	~CGltfBufferView() = default;
	
	[[nodiscard]] constexpr _STD uint8_t const& operator[](_STD size_t const index) const {
		assert(index + offset_ <= buffer_->length());
		return buffer_->operator[](index + offset_);
	}

	[[nodiscard]] constexpr ETarget target() const { return target_; }
	[[nodiscard]] constexpr _STD uint32_t offset() const { return offset_; }
	[[nodiscard]] constexpr _STD uint32_t length() const { return length_; }

private:
	_STD shared_ptr<CGltfBuffer> buffer_;
	_STD uint32_t length_ = 0u, offset_ = 0u;
	ETarget target_ = ETarget::ARRAY;
};

enum class GltfBufferViewTarget_e : _STD uint16_t {
	ARRAY = 34962,
	ELEMENT = 34963
};

// we dont need a fancy thing
struct GltfBufferView_t {
	gltf::id buffer = 0;
	gltf::size length = 0u, offset = 0u;
	_STD optional<GltfBufferViewTarget_e> target = _STD nullopt;
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
	_STD variant<
		GltfCameraOrthographic_t,
		GltfCameraPerspective_t
	> camera;

	[[nodiscard]] constexpr bool is_orthographic() const{
		return _STD holds_alternative<GltfCameraOrthographic_t>(camera);
	}
	[[nodiscard]] constexpr bool is_perspective() const{
		return _STD holds_alternative<GltfCameraPerspective_t>(camera);
	}
	[[nodiscard]] constexpr GltfCameraOrthographic_t const& orthographic() const {
		return _STD get<GltfCameraOrthographic_t>(camera);
	}
	[[nodiscard]] constexpr GltfCameraPerspective_t const& perspective() const {
		return _STD get<GltfCameraPerspective_t>(camera);
	}
};

#ifdef GLTF_THREADED_IMAGE_LOADING
extern _STD vector<_STD thread> gltf_worker_threads_;

struct GltfImage_t {
	_STD string name;
	_STD string mimeType;
	_STD string uri; //< If this is empty, check bufferView!
	gltf::id bufferView; //< Ensure that URI is unused!
	_STD future<_STD shared_ptr<_STD vector<unsigned char>>> external_data;
};
#else

struct GltfImage_t {
	_STD string name;
	_STD string mimeType;
	_STD string uri; //< If this is empty, check bufferView!
	gltf::id bufferView; //< Ensure that URI is unused!
	_STD vector<unsigned char> external_data;
	gltf::id channels; //< Not a part of the glTF spec, but is used to share the information from assembling buffers & images to the gpu alloc stage.
	glm::ivec2 size;
};
#endif

struct PbrMetallicRoughness_t {
	gltf::id base_color_texture;
	gltf::id metallic_roughness_texture;
};

struct GltfMaterial_t {
	_STD string name;
	gltf::id emissive_texture;
	gltf::id normal_texture;
	gltf::id occlusion_texture;
	PbrMetallicRoughness_t pbr_metallic_roughness;
};

struct GltfNode_t {
	_STD string name;
	//union {
		glm::mat4 matrix;
	///	struct {
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 scale;
	//	};
	//};
	bool has_transform = false;
	_STD vector<gltf::id> children;
	_STD vector<GLTF_NUMBER> weights;
	gltf::id camera = -1;
	gltf::id skin = -1;
	gltf::id mesh = -1;
};

struct GltfScene_t {
	_STD string name;
	_STD vector<gltf::id> nodes;
};

struct GltfMeshPrimitiveAttrib_t {
	_STD string name;
	gltf::id accessor;
};

using GltfMeshPrimitiveAttributes = _STD vector<GltfMeshPrimitiveAttrib_t>;

enum class GltfMeshPrimitiveMode_e : _STD uint8_t {
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

using GltfMeshPrimitives = _STD vector<GltfMeshPrimitive_t>;

struct GltfMesh_t {
	_STD string name;
	GltfMeshPrimitives primitives;
	_STD vector<GLTF_NUMBER> weights; //< MUST be same size as morph targets.
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

struct GltfSkin_t {
	gltf::id inverseBindMatrices = -1; //< Points to Accessor
	_STD vector<gltf::id> joints; //< Pointers to Nodes
	_STD string name; //< Name.
};

using GltfBuffers = _STD vector<CGltfBuffer>;
using GltfAccessors = _STD vector<CGltfAccessor>;
using GltfBufferViews = _STD vector<GltfBufferView_t>;
using GltfCameras = _STD vector<GltfCamera_t>;
using GltfImages = _STD vector<GltfImage_t>;
using GltfNodes = _STD vector<GltfNode_t>;
using GltfMeshes = _STD vector<GltfMesh_t>;
using GltfTextures = _STD vector<GltfTexture_t>;
using GltfSamplers = _STD vector<GltfSampler_t>;
using GltfScenes = _STD vector<GltfScene_t>;
using GltfSkins = _STD vector<GltfSkin_t>;
using GltfMaterials = _STD vector<GltfMaterial_t>;

struct GltfData_t {
	_STD filesystem::path	path;
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
	GltfSkins				skins;
	GltfMaterials			materials;
	gltf::id				scene;
};

struct GltfFile_t {
	_STD fstream file;
};

namespace gltf {
	extern GltfData_t parse(_STD string const& file_path, simdjson::padded_string &&file);
}