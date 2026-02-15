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



#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "types.hpp"
#include "simdjson/simdjson.h"

namespace gltf {
	#ifdef GLTF_NUMBER_IS_DOUBLE
	using number = std::double_t;
	#else
	using number = std::double_t;
	#endif

	using id = std::int32_t;
}

enum class EGltfComponentType : std::uint8_t {
	BYTE,
	UBYTE,
	SHORT,
	USHORT,
	FLOAT
};
constexpr char const *to_string(EGltfComponentType e) {
	switch (e) {
		case EGltfComponentType::BYTE: return "BYTE";
		case EGltfComponentType::UBYTE: return "UBYTE";
		case EGltfComponentType::SHORT: return "SHORT";
		case EGltfComponentType::USHORT: return "USHORT";
		case EGltfComponentType::FLOAT: return "FLOAT";
	}
	return "unknown"; // <--- stupid shit to shut up the ide
}

enum class EGltfType : std::uint8_t {
	SCALAR,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4,
};

constexpr char const *to_string(EGltfType e) {
	switch (e) {
		case EGltfType::SCALAR: return "SCALAR";
		case EGltfType::VEC2: return "VEC2";
		case EGltfType::VEC3: return "VEC3";
		case EGltfType::VEC4: return "VEC4";
		case EGltfType::MAT2: return "MAT2";
		case EGltfType::MAT3: return "MAT3";
		case EGltfType::MAT4: return "MAT4";
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

	void setComponentType(EGltfComponentType p_type);
	[[nodiscard]] EGltfComponentType componentType() const;

	void setType(EGltfType p_type);
	[[nodiscard]] EGltfType type() const;
	
	void setBufferView(gltf::id p_bufferViewIndex);
	[[nodiscard]] gltf::id bufferView() const;

	void setOffset(std::uint32_t p_offset);
	[[nodiscard]] std::uint32_t offset() const;

	void setCount(std::uint32_t p_count);
	[[nodiscard]] std::uint32_t count() const;

	void setMax(std::array<gltf::number, 16> const& p_max);
	void setMaxComponent(std::size_t p_index, gltf::number p_value);
	[[nodiscard]] std::array<gltf::number, 16> const& max() const;
	
	void setMin(std::array<gltf::number, 16> const& p_min);
	void setMinComponent(std::size_t p_index, gltf::number p_value);
	[[nodiscard]] std::array<gltf::number, 16> const& min() const;
	
private:
	EGltfComponentType component_type_ = EGltfComponentType::BYTE;
	EGltfType type_ = EGltfType::SCALAR;
	std::array<gltf::number, 16> max_;
	std::array<gltf::number, 16> min_;
	gltf::id buffer_view_ = 0u;
	std::uint32_t offset_ = 0u;
	std::uint32_t count_ = 0u;
	bool normalized_ = false;
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

struct GltfCameraOrthographic_t {
	gltf::number
		x_magnification,
		y_magnification,
		z_near, z_far;
};

struct GltfCameraPerspective_t {
	gltf::number
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

struct GltfImage_t {
	std::string uri;
	std::string mimeType;
	std::string name;
	gltf::id bufferView;
};

struct GltfNode_t {
	std::string name;
	glm::mat4 matrix;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 scale;
	std::vector<gltf::id> children;
	std::vector<gltf::number> weights;
	gltf::id camera = -1;
	gltf::id skin = -1;
	gltf::id mesh = -1;
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
	GltfMeshPrimitiveMode_e mode = GltfMeshPrimitiveMode_e::TRIANGLES;
	
};

using GltfMeshPrimitives = std::vector<GltfMeshPrimitive_t>;

struct GltfMesh_t {
	std::string name;
	GltfMeshPrimitives primitives;
	std::vector<gltf::number> weights; //< MUST be same size as morph targets.
};

using GltfAccessors = std::vector<CGltfAccessor>;
using GltfBuffers = std::vector<CGltfBuffer>;
using GltfBufferViews = std::vector<CGltfBufferView>;
using GltfCameras = std::vector<GltfCamera_t>;
using GltfImages = std::vector<GltfImage_t>;
using GltfNodes = std::vector<GltfNode_t>;
using GltfMeshes = std::vector<GltfMesh_t>;

struct GltfData_t {
	GltfAccessors	accessors;
	GltfBuffers		buffers;
	GltfCameras		cameras;
	GltfImages		images;
	GltfNodes		nodes;
	GltfMeshes		meshes;
};

struct GltfFile_t {
	std::fstream file;
};

namespace gltf {
	extern GltfData_t parse(simdjson::padded_string &&file);
}