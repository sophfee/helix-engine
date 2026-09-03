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
#include "glad/glad.h"
#include "khr/ktx.h"

#define GLTF_DEBUG 0

class Material;

using TextureMagFilter = vk::SamplerAddressMode;

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
#include "gltf/KHR_lights_punctual.hpp"

namespace gltf {
	#ifdef GLTF_NUMBER_IS_DOUBLE
	using number = _STD double_t;
	#else
	using number = _STD float_t;
	#endif

	using id = _STD int32_t;
	using size = _STD size_t;

	enum class Component : _STD uint8_t {
		eSignedByte,
		eUnsignedByte,
		eSignedShort,
		eUnsignedShort,
		eSignedInt,
		eUnsignedInt,
		eFloat
	};
	constexpr char const *to_string(Component e) {
		switch (e) {
			case Component::eSignedByte: return "Signed Byte";
			case Component::eUnsignedByte: return "Unsigned Byte";
			case Component::eSignedShort: return "Signed Short";
			case Component::eUnsignedShort: return "Unsigned Short";
			case Component::eFloat: return "Float";
		default: return "unknown";
		}
		return "unknown"; // <--- stupid shit to shut up the ide
	}

	constexpr static u32 to_gl(Component e) {
		switch (e) {
			case Component::eSignedByte: return GL_BYTE;
			case Component::eUnsignedByte: return GL_UNSIGNED_BYTE;
			case Component::eSignedShort: return GL_SHORT;
			case Component::eUnsignedShort: return GL_UNSIGNED_SHORT;
			case Component::eFloat: return GL_FLOAT;
		}
		return GL_NONE;
	} 

	enum class Type : _STD uint8_t {
		eScalar,
		eFloat2,
		eFloat3,
		eFloat4,
		eFloat2x2,
		eFloat3x3,
		eFloat4x4,
	};

	constexpr int get_components_in_type(Type const t) {
		switch (t) {
			case Type::eScalar: return 1;
			case Type::eFloat2: return 2;
			case Type::eFloat3: return 3;
			case Type::eFloat4:
			case Type::eFloat2x2: return 4;
			case Type::eFloat3x3: return 9;
			case Type::eFloat4x4: return 16;
		}
		return 0;
	}

	constexpr size get_size_for_component_type(Component const ct) {
		switch (ct) {
			case Component::eSignedByte: return sizeof(i8);
			case Component::eUnsignedByte: return sizeof(u8);
			case Component::eSignedShort: return sizeof(i16);
			case Component::eUnsignedShort: return sizeof(u16);
			case Component::eSignedInt: return sizeof(i32);
			case Component::eUnsignedInt: return sizeof(u32);
			case Component::eFloat: return sizeof(number);
		}
		return sizeof(number);
	}
	
	constexpr ComponentType convert(Component const ct) {
		switch (ct) {
			case Component::eSignedByte: return ComponentType::SIGNED_BYTE;
			case Component::eUnsignedByte: return ComponentType::UNSIGNED_BYTE;
			case Component::eSignedShort: return ComponentType::SIGNED_SHORT;
			case Component::eUnsignedShort: return ComponentType::UNSIGNED_SHORT;
			case Component::eFloat: return ComponentType::SINGLE_FLOAT;
		}
		return ComponentType::SINGLE_FLOAT;
	}
	
	constexpr char const *to_string(Type e) {
		switch (e) {
			case Type::eScalar: return "Scalar";
			case Type::eFloat2: return "Float2";
			case Type::eFloat3: return "Float3";
			case Type::eFloat4: return "Float4";
			case Type::eFloat2x2: return "Float 2x2";
			case Type::eFloat3x3: return "Float 3x3";
			case Type::eFloat4x4: return "Float 4x4";
		}
		return "unknown"; // <--- stupid shit to shut up the ide
	}

	class Property {
	public:
		virtual ~Property() = default;

		void setName(_STD string const& p_name);
		[[nodiscard]] _STD string const& name() const;

	private:
		_STD string name_;
	};

	/**
	 * 
	 */
	class Accessor : public Property {
	public:

		Accessor();
		~Accessor();

		void set_component_type(Component p_type);
		[[nodiscard]] Component get_component_type() const;

		void set_type(Type p_type);
		[[nodiscard]] Type get_type() const;
	
		void set_buffer_view(id p_bufferViewIndex);
		[[nodiscard]] id get_buffer_view() const;

		void set_offset(size p_offset);
		[[nodiscard]] size get_offset() const;

		void set_count(size p_count);
		[[nodiscard]] size get_count() const;

		constexpr size_t get_stride() const;

		void set_max(_STD array<GLTF_NUMBER, 16> const& p_max);
		void set_max_component(_STD size_t p_index, GLTF_NUMBER p_value);
		[[nodiscard]] _STD array<GLTF_NUMBER, 16> const& get_max() const;
	
		void set_min(_STD array<GLTF_NUMBER, 16> const& p_min);
		void set_min_component(_STD size_t p_index, GLTF_NUMBER p_value);
		[[nodiscard]] _STD array<GLTF_NUMBER, 16> const& get_min() const;
	
	private:
		Component component_type_ = Component::eSignedByte;
		gltf::Type type_ = Type::eScalar;
		_STD array<GLTF_NUMBER, 16> max_;
		_STD array<GLTF_NUMBER, 16> min_;
		id buffer_view_ = 0u;
		size offset_ = 0u;
		size count_ = 0u;
		bool normalized_ = false;
	};
	
	constexpr size_t Accessor::get_stride() const {
		return get_size_for_component_type(component_type_) * get_components_in_type(type_);
	}

	struct gltf_accessor {
		Component component_type = Component::eSignedByte;
		Type type = Type::eScalar;
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
	class Buffer {
	public:

		Buffer() = default;
		Buffer(_STD string const& uri, _STD string const& name);
		Buffer(_STD vector<char> &&data);

		[[nodiscard]] constexpr char const& operator[](_STD size_t const index) const {
			return data_[index];
		}

		[[nodiscard]] constexpr size get_length() const { return data_.size(); }

		inline _STD vector<char> const& get_data() const { return data_; }
		inline _STD vector<char>& get_data() { return data_; }
		inline _STD string get_uri() const noexcept { return uri_.value_or(""); }
		inline _STD string get_name() const noexcept { return name_.value_or(""); }
	
	private:
		_STD vector<char> data_;
		_STD optional<_STD string> uri_, name_;
	};

	enum class BufferViewTarget : _STD uint16_t {
		ARRAY = 34962,
		ELEMENT = 34963
	};

	// we dont need a fancy thing
	struct BufferView {
		id buffer = 0;
		size length = 0u, offset = 0u, stride = 0u;
		_STD optional<BufferViewTarget> target = _STD nullopt;
		_STD uint8_t *data = nullptr;
	};

	struct CameraOrthographic {
		number
			x_magnification,
			y_magnification,
			z_near, z_far;
	};

	struct CameraPerspective {
		number
			aspect_ratio, y_fov,
			z_far, z_near;
	};

	struct Camera {
		_STD variant<
			CameraOrthographic,
			CameraPerspective
		> data;

		[[nodiscard]] constexpr bool is_orthographic() const{
			return _STD holds_alternative<CameraOrthographic>(data);
		}
		[[nodiscard]] constexpr bool is_perspective() const{
			return _STD holds_alternative<CameraPerspective>(data);
		}
		[[nodiscard]] constexpr CameraOrthographic const& get_orthographic() const {
			return _STD get<CameraOrthographic>(data);
		}
		[[nodiscard]] constexpr CameraPerspective const& get_perspective() const {
			return _STD get<CameraPerspective>(data);
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

	enum ImageType {
		ePNG,
		eDDS,
		eKTX2,
		eGeneric
	};

	struct Image {
		_STD string name;
		_STD string mime_type;
		ImageType image_type;
		_STD string uri; //< If this is empty, check bufferView!
		id channels; //< Not a part of the glTF spec, but is used to share the information from assembling buffers & images to the gpu alloc stage.
		id buffer_view; //< Ensure that URI is unused!
		u32 hash_value;
		bool compressed;
		ivec2 size;
		std::shared_ptr<std::vector<u8>> external_data;
		ktxTexture *ktx2_texture;
		bool is_ktx2;
		bool is_dds;
		std::string file;
	};
#endif

	struct TextureInfo {
		id index = 0;
		id tex_coord = 0;
		number scale = 1.0f;
		bool exists = false;
	};

	enum class AlphaMode : _STD uint8_t {
		eOpaque,
		eMask,
		eBlend
	};

	struct PbrMetallicRoughness {
		TextureInfo base_color_texture;
		vec4 base_color_factor = vec4(1.0f);
	
		TextureInfo metallic_roughness_texture;
		number metallic_factor  = 0.00f;
		number roughness_factor = 0.75f;
	};
	struct Material {
		_STD string name;
		vec4 emissive_factor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		TextureInfo emissive_texture;
		TextureInfo normal_texture;
		TextureInfo occlusion_texture;
		PbrMetallicRoughness pbr_metallic_roughness;
		bool double_sided = false;
		number alpha_cutoff = 0.5;
		AlphaMode alpha_mode = AlphaMode::eOpaque;
		SharedPtr<::Material> impl;
	};

	struct Node {
		_STD string name;
		//union {
		mat4 matrix;
		///	struct {
		quat rotation;
		vec3 translation;
		vec3 scale;
		//	};
		//};
		bool has_transform = false;
		_STD vector<id> children;
		_STD vector<GLTF_NUMBER> weights;
		id camera = -1;
		id skin = -1;
		id mesh = -1;

		struct {
			_STD optional<khr::lights_punctual::node> KHR_lights_punctual;
		} extensions;
	};

	struct Scene {
		_STD string name;
		_STD vector<id> nodes;
	};

	struct Attribute {
		_STD string name;
		id accessor;
	};

	using Attributes = _STD vector<Attribute>;

	enum class PrimitiveMode : _STD uint8_t {
		ePoints = 0,
		eLines,
		eLineLoop,
		eLineStrip,
		eTriangles,
		eTriangleStrip,
		eTriangleFan,
	};

	struct Primitive {
		Attributes attributes;
		i32 indices = -1;
		u32 material = 0;
		PrimitiveMode mode = PrimitiveMode::eTriangles;
	};

	using Primitives = _STD vector<Primitive>;

	struct Mesh {
		_STD string name;
		Primitives primitives;
		_STD vector<GLTF_NUMBER> weights; //< MUST be same size as morph targets.
	};

	struct Texture {
		id sampler;
		id source; //< Image
		SharedPtr<RID> impl;
		bool impl_exists = false;
	};

	struct Sampler {
		vk::SamplerMipmapMode mag_filter = vk::SamplerMipmapMode::eLinear;
		vk::SamplerMipmapMode min_filter = vk::SamplerMipmapMode::eLinear;
		vk::SamplerAddressMode wrap_s_mode = vk::SamplerAddressMode::eRepeat;
		vk::SamplerAddressMode wrap_t_mode = vk::SamplerAddressMode::eRepeat;
	};

	struct skin {
		id inverseBindMatrices = -1; //< Points to Accessor
		_STD vector<id> joints; //< Pointers to Nodes
		_STD string name; //< Name.
	};

	using Buffers = _STD vector<Buffer>;
	using Accessors = _STD vector<Accessor>;
	using BufferViews = _STD vector<BufferView>;
	using Cameras = _STD vector<Camera>;
	using Images = _STD vector<Image>;
	using Nodes = _STD vector<Node>;
	using Meshes = _STD vector<Mesh>;
	using Textures = _STD vector<Texture>;
	using Samplers = _STD vector<Sampler>;
	using Scenes = _STD vector<Scene>;
	using Skins = _STD vector<skin>;
	using Materials = _STD vector<Material>;

	struct Extensions {
		std::optional<khr::lights_punctual::global> KHR_lights_punctual;
	};

	struct Data {
		_STD filesystem::path	path;
		Buffers			buffers;
		BufferViews	buffer_views;
		Accessors		accessors;
		Cameras			cameras;
		Images			images;
		Nodes			nodes;
		Meshes			meshes;
		Textures		textures;
		Samplers		samplers;
		Scenes			scenes;
		Skins			skins;
		Materials		materials;
		id	scene;
		Extensions	extensions;

		[[nodiscard]] u32 get_accessor_count(i32 const accessorIndex) const { return accessors[accessorIndex].get_count(); }

		template <typename T>
		[[nodiscard]] T *make_cursor(i32 const accessorIndex, i32 const valueIndex) {
			Accessor & acc = accessors[accessorIndex];
			BufferView & bv = buffer_views[acc.get_buffer_view()];
			Buffer & buf = buffers[bv.buffer];
			assert(buf.get_length() >= bv.offset + bv.length);
			return &reinterpret_cast<T *>(buf.get_data().data() + bv.offset + acc.get_offset())[valueIndex];
		}

		template <typename T>
		[[nodiscard]] T const &read_accessor(i32 const accessorIndex, i32 const valueIndex) {
			Accessor & acc = accessors[accessorIndex];
			BufferView & bv = buffer_views[acc.get_buffer_view()];
			Buffer & buf = buffers[bv.buffer];
			assert(buf.get_length() >= bv.offset + bv.length);
			return reinterpret_cast<T *>(buf.get_data().data() + bv.offset + acc.get_offset())[valueIndex];
		}
	};

	struct GltfFile {
		_STD fstream file;
	};

	extern Data parse(_STD string const& file_path, simdjson::padded_string &&file);
}