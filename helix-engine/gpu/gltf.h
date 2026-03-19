#pragma once

#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <future>

#include "graphics.hpp"
#include "opengl_enums.hpp"
#include "png.h"
#include "glad/glad.h"

#define GLTF_DEBUG 0

namespace gl {
	enum class TextureMagFilter : enum_t;
}

#if GLTF_DEBUG == 1
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
#include "simdjson/simdjson.h"

namespace gltf {
	#ifdef GLTF_NUMBER_IS_DOUBLE
	using number = _STD double_t;
	#else
	using number = _STD float_t;
	#endif

	using id = _STD int32_t;
	using size = _STD size_t;

	enum class component_type : _STD uint8_t {
		signed_byte,
		unsigned_byte,
		signed_short,
		unsigned_short,
		single_float
	};
	constexpr char const *to_string(component_type e) {
		switch (e) {
			case component_type::signed_byte: return "signed_byte";
			case component_type::unsigned_byte: return "unsigned_byte";
			case component_type::signed_short: return "signed_short";
			case component_type::unsigned_short: return "unsigned_short";
			case component_type::single_float: return "single_float";
		}
		return "unknown"; // <--- stupid shit to shut up the ide
	}

	constexpr static u32 componentTypeToGL(component_type e) {
		switch (e) {
			case component_type::signed_byte: return GL_BYTE;
			case component_type::unsigned_byte: return GL_UNSIGNED_BYTE;
			case component_type::signed_short: return GL_SHORT;
			case component_type::unsigned_short: return GL_UNSIGNED_SHORT;
			case component_type::single_float: return GL_FLOAT;
		}
		return GL_NONE;
	} 

	enum class type : _STD uint8_t {
		scalar,
		vec2,
		vec3,
		vec4,
		mat2,
		mat3,
		mat4,
	};


	constexpr int componentsForType(type const t) {
		switch (t) {
			case type::scalar: return 1;
			case type::vec2: return 2;
			case type::vec3: return 3;
			case type::vec4:
			case type::mat2: return 4;
			case type::mat3: return 9;
			case type::mat4: return 16;
		}
		return 0;
	}

	constexpr size sizeForComponentType(component_type const ct) {
		switch (ct) {
			case component_type::signed_byte: return sizeof(i8);
			case component_type::unsigned_byte: return sizeof(u8);
			case component_type::signed_short: return sizeof(i16);
			case component_type::unsigned_short: return sizeof(u16);
			case component_type::single_float: return sizeof(number);
		}
		return sizeof(number);
	}
	
	constexpr EComponentType gpuComponentTypeFromGltfComponentType(component_type const ct) {
		switch (ct) {
			case component_type::signed_byte: return EComponentType::SIGNED_BYTE;
			case component_type::unsigned_byte: return EComponentType::UNSIGNED_BYTE;
			case component_type::signed_short: return EComponentType::SIGNED_SHORT;
			case component_type::unsigned_short: return EComponentType::UNSIGNED_SHORT;
			case component_type::single_float: return EComponentType::SINGLE_FLOAT;
		}
		return EComponentType::SINGLE_FLOAT;
	}
	
	constexpr char const *to_string(type e) {
		switch (e) {
			case type::scalar: return "scalar";
			case type::vec2: return "vec2";
			case type::vec3: return "vec3";
			case type::vec4: return "vec4";
			case type::mat2: return "mat2";
			case type::mat3: return "mat3";
			case type::mat4: return "mat4";
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
	class accessor : public CGltfProperty {
	public:

		accessor();
		~accessor();

		void setComponentType(component_type p_type);
		[[nodiscard]] component_type componentType() const;

		void setType(type p_type);
		[[nodiscard]] type type() const;
	
		void setBufferView(id p_bufferViewIndex);
		[[nodiscard]] id bufferView() const;

		void setOffset(size p_offset);
		[[nodiscard]]  size offset() const;

		void setCount(size p_count);
		[[nodiscard]] size count() const;

		void setMax(_STD array<GLTF_NUMBER, 16> const& p_max);
		void setMaxComponent(_STD size_t p_index, GLTF_NUMBER p_value);
		[[nodiscard]] _STD array<GLTF_NUMBER, 16> const& max() const;
	
		void setMin(_STD array<GLTF_NUMBER, 16> const& p_min);
		void setMinComponent(_STD size_t p_index, GLTF_NUMBER p_value);
		[[nodiscard]] _STD array<GLTF_NUMBER, 16> const& min() const;
	
	private:
		component_type component_type_ = component_type::signed_byte;
		gltf::type type_ = type::scalar;
		_STD array<GLTF_NUMBER, 16> max_;
		_STD array<GLTF_NUMBER, 16> min_;
		id buffer_view_ = 0u;
		size offset_ = 0u;
		size count_ = 0u;
		bool normalized_ = false;
	};

	struct gltf_accessor {
		component_type component_type = component_type::signed_byte;
		type type = type::scalar;
		_STD array<GLTF_NUMBER, 16> max;
		_STD array<GLTF_NUMBER, 16> min;
		id buffer_view = 0u;
		size offset = 0u;
		size count = 0u;
		bool normalized = false;
	};

	/**
	 * Buf
	 * @section 5.10
	 */
	class buffer {
	public:

		buffer() = default;
		buffer(_STD string const& uri, _STD string const& name);
		buffer(_STD vector<char> &&data);

		[[nodiscard]] constexpr char const& operator[](_STD size_t const index) const {
			return data_[index];
		}

		[[nodiscard]] constexpr size length() const { return data_.size(); }

		inline _STD vector<char> const& data() const { return data_; }
		inline _STD string uri() const noexcept { return uri_.value_or(""); }
		inline _STD string name() const noexcept { return name_.value_or(""); }
	
	private:
		_STD vector<char> data_;
		_STD optional<_STD string> uri_, name_;
	};

	enum class buffer_view_target : _STD uint16_t {
		ARRAY = 34962,
		ELEMENT = 34963
	};

	// we dont need a fancy thing
	struct buffer_view {
		id buffer = 0;
		size length = 0u, offset = 0u;
		_STD optional<buffer_view_target> target = _STD nullopt;
		_STD uint8_t *data = nullptr;
	};

	struct camera_orthographic {
		number
			x_magnification,
			y_magnification,
			z_near, z_far;
	};

	struct camera_perspective {
		number
			aspect_ratio, y_fov,
			z_far, z_near;
	};

	struct camera {
		_STD variant<
			camera_orthographic,
			camera_perspective
		> data;

		[[nodiscard]] constexpr bool is_orthographic() const{
			return _STD holds_alternative<camera_orthographic>(data);
		}
		[[nodiscard]] constexpr bool is_perspective() const{
			return _STD holds_alternative<camera_perspective>(data);
		}
		[[nodiscard]] constexpr camera_orthographic const& orthographic() const {
			return _STD get<camera_orthographic>(data);
		}
		[[nodiscard]] constexpr camera_perspective const& perspective() const {
			return _STD get<camera_perspective>(data);
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

	struct image {
		_STD string name;
		_STD string mimeType;
		_STD string uri; //< If this is empty, check bufferView!
		id channels; //< Not a part of the glTF spec, but is used to share the information from assembling buffers & images to the gpu alloc stage.
		id bufferView; //< Ensure that URI is unused!
		u32 hash_value;
		bool compressed;
		glm::ivec2 size;
		std::shared_ptr<std::vector<u8>> external_data;
	};
#endif

	struct texture_info {
		id index;
		id tex_coord;
		number scale;
	};

	enum class alpha_mode : _STD uint8_t {
		opaque,
		mask,
		blend
	};

	struct pbr_metallic_roughness {
		texture_info base_color_texture;
		glm::vec4 base_color_factor;
	
		texture_info metallic_roughness_texture;
		number metallic_factor;
		number roughness_factor;
	};

	struct material {
		_STD string name;
		glm::vec4 emissive_factor;
		texture_info emissive_texture;
		texture_info normal_texture;
		texture_info occlusion_texture;
		pbr_metallic_roughness pbr_metallic_roughness;
		bool double_sized = false;
		number alpha_cutoff = 0.5;
		alpha_mode alpha_mode = alpha_mode::opaque;
	};

	struct node {
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
		_STD vector<id> children;
		_STD vector<GLTF_NUMBER> weights;
		id camera = -1;
		id skin = -1;
		id mesh = -1;
	};

	struct scene {
		_STD string name;
		_STD vector<id> nodes;
	};

	struct attribute {
		_STD string name;
		id accessor;
	};

	using attributes = _STD vector<attribute>;

	enum class primitive_mode : _STD uint8_t {
		points = 0,
		lines,
		line_loop,
		line_strip,
		triangles,
		triangle_strip,
		triangle_fan,
	};

	struct primitive {
		attributes attributes;
		i32 indices = -1;
		u32 material = 0;
		primitive_mode mode = primitive_mode::triangles;
	};

	using primitives = _STD vector<primitive>;

	struct mesh {
		_STD string name;
		primitives primitives;
		_STD vector<GLTF_NUMBER> weights; //< MUST be same size as morph targets.
	};

	struct texture {
		id sampler;
		id source; //< Image
		CSharedPtr<CTexture> impl;
	};

	struct sampler {
		gl::TextureMagFilter mag_filter = gl::TextureMagFilter::Linear;
		gl::TextureMinFilter min_filter = gl::TextureMinFilter::Linear;
		gl::TextureWrapMode wrap_s_mode = gl::TextureWrapMode::Clamp;
		gl::TextureWrapMode wrap_t_mode = gl::TextureWrapMode::Clamp;
	};

	struct skin {
		id inverseBindMatrices = -1; //< Points to Accessor
		_STD vector<id> joints; //< Pointers to Nodes
		_STD string name; //< Name.
	};

	using buffers = _STD vector<buffer>;
	using accessors = _STD vector<accessor>;
	using buffer_views = _STD vector<buffer_view>;
	using cameras = _STD vector<camera>;
	using images = _STD vector<image>;
	using nodes = _STD vector<node>;
	using meshes = _STD vector<mesh>;
	using textures = _STD vector<texture>;
	using samplers = _STD vector<sampler>;
	using scenes = _STD vector<scene>;
	using skins = _STD vector<skin>;
	using materials = _STD vector<material>;

	struct data {
		_STD filesystem::path	path;
		buffers				buffers;
		buffer_views			buffer_views;
		accessors			accessors;
		cameras				cameras;
		images				images;
		nodes				nodes;
		meshes				meshes;
		textures			textures;
		samplers			samplers;
		scenes				scenes;
		skins				skins;
		materials			materials;
		id				scene;
	};

	struct GltfFile {
		_STD fstream file;
	};

	extern data parse(_STD string const& file_path, simdjson::padded_string &&file);
}