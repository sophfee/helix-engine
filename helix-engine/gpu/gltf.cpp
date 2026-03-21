// ReSharper disable CppCStyleCast
// ReSharper disable CppClangTidyBugproneUnsafeFunctions
#include "gltf.h"
#include "simdjson/simdjson.h"

#include <cassert>
#include <fstream>

#include "png.hpp"
#include "stb/stb_image.h"
#include "libpng/png.h"

#include "types.hpp"

using namespace gltf;

void CGltfProperty::setName(_STD string const &p_name) { name_ = p_name; }
_STD string const & CGltfProperty::name() const { return name_; }

// Gltf Accessor

accessor::accessor() = default;
accessor::~accessor() {}

void accessor::setComponentType(component_type const p_type) { component_type_ = p_type; }
component_type accessor::componentType() const { return component_type_; }

void accessor::setType(enum type const p_type) { type_ = p_type; }
enum type accessor::type() const { return type_; }

void accessor::setBufferView(id const p_bufferViewIndex) { buffer_view_ = p_bufferViewIndex; }
id accessor::bufferView() const { return buffer_view_; }

void accessor::setOffset(size const p_offset) { offset_ = p_offset; }
size accessor::offset() const { return offset_; }

void accessor::setCount(size const p_count) { count_ = p_count; }
size accessor::count() const { return count_; }

void accessor::setMax(_STD array<GLTF_NUMBER, 16> const &p_max) { max_ = p_max; }
void accessor::setMaxComponent(_STD size_t const p_index, GLTF_NUMBER const p_value) { max_[p_index] = p_value; }
_STD array<GLTF_NUMBER, 16> const & accessor::max() const { return max_; }

void accessor::setMin(_STD array<GLTF_NUMBER, 16> const &p_min) { min_ = p_min; }
void accessor::setMinComponent(_STD size_t const p_index, GLTF_NUMBER const p_value) { min_[p_index] = p_value; }
_STD array<GLTF_NUMBER, 16> const & accessor::min() const { return min_; }

buffer::buffer(_STD string const& uri, _STD string const& name)
	: uri_(uri), name_(name) {
	
#ifdef GLTF_USE_STD_FILESYSTEM
	_STD fstream file(uri_.value(), _STD ios::binary);

	file.seekg(0, _STD ios::end);
	_STD cout << file.tellg() << '\n';
	data_.resize(file.tellg());
	file.seekg(0, _STD ios::beg);

	file.read(
		reinterpret_cast<char *>(data_.data()),
		static_cast<_STD streamsize>(data_.size())
	);
#else
	_STD cout << "GltfBuffer: Loading buffer from URI: " << uri << '\n';
	FILE *file;
	errno_t r = fopen_s(&file, uri.c_str(), "rb");
	assert(r == 0);

	(void)fseek(file, 0, SEEK_END);
	_STD cout << ftell(file) << '\n';
	data_.resize(ftell(file));
	(void)fseek(file, 0, SEEK_SET);
	(void)fread(
		data_.data(),
		sizeof(char),
		data_.size(),
		file
	);
	(void)fclose(file);
#endif
}

buffer::buffer(_STD vector<char> &&data) : data_(_STD move(data)) {
}

//
//
// MAIN ACTUAL PARSING IS BELOW!!
//
//
using namespace simdjson;

_STD vector<_STD thread> gltf_worker_threads_;

namespace {
	accessor parse_accessor(ondemand::value &accessor) {
		gltf::accessor a;
		simdjson_result<ondemand::value> bv = accessor["bufferView"];
		simdjson_result<ondemand::value> ct = accessor["componentType"];
		simdjson_result<ondemand::value> t = accessor["type"];
		simdjson_result<ondemand::value> mn = accessor["min"];
		simdjson_result<ondemand::value> max = accessor["max"];
		simdjson_result<ondemand::value> cn = accessor["count"];

#ifdef GLTF_VERBOSE_DEBUG
		gltfDebugPrintf("[Accessor] bufferView is valid %s", bv.has_value() ? "true" : "false");
		gltfDebugPrintf("[Accessor] componentType: %s", ct.has_value() ? "true" : "false");
		gltfDebugPrintf("[Accessor] type: %s", t.has_value() ? "true" : "false");
#endif
		
		if (bv.has_value()) a.setBufferView(static_cast<id>(bv.get_int64().value()));
		if (ct.has_value()) {
			switch(auto const ctv = ct.get_int64().value()) {
				case 5126:
					a.setComponentType(component_type::single_float);
					break;
				case 5125:
					a.setComponentType(component_type::unsigned_short);
					break;
				case 5121:
					a.setComponentType(component_type::unsigned_byte);
					break;
				default: break;
			}
		}

		if (t.has_value()) {
			if (_STD string_view const text = t.get_string().value(); text.length() == 6) {
				a.setType(type::scalar); // scalar is 6 letters lol, fun little way to optimize
			}
			else {
				char const initial = text[0];
				char const number = text[3];
				switch (initial) {
					case 'V': {
						switch (number) {
							case '2': {
								a.setType(type::vec2);
								break;
							}
							case '3': {
								a.setType(type::vec3);
								break;
							}
							case '4': {
								a.setType(type::vec4);
								break;
							}
							default:
								break;
						}
						break;
					}
					case 'M': {
						switch (number) {
							case '2': {
								a.setType(type::mat2);
								break;
							}
							case '3': {
								a.setType(type::mat3);
								break;
							}
							case '4': {
								a.setType(type::mat4);
								break;
							}
							default: break;
						}
						break;
					}
					default: break;
				}
			}
		}

		if (cn.has_value()) {
			a.setCount(static_cast<_STD uint32_t>(cn.get_int64().value()));
		}

		return (a);
	}

	buffer parse_buffer(_STD filesystem::path &root, ondemand::value &object) {
		if (auto uri = object["uri"]; uri.has_value()) {
			gltfDebugPrint("Buffer contains a uri, not inline data.");
			auto const text = uri.get_string().value();
			auto const chars = new char[text.length() + 1];
			_STD memset(chars, 0, text.length() + 1);
			text.copy(chars, text.length());
			gltfDebugPrintf("Buffer's URI is \"%s\"", chars);
#ifdef GLTF_USE_STD_FILESYSTEM
			_STD fstream file(chars, _STD ios::in | _STD ios::binary);
			gltfDebugPrintf("file.is_open() = %s", file.is_open() ? "TRUE" : "FALSE");

			if (!file.is_open()) {
				// Try appending the epic root directory
				file = _STD fstream(root / chars, _STD ios::in | _STD ios::binary);
			}
			delete[] chars;
		
			_STD vector<char> data;
			file.seekg(0, _STD ios::end);
			data.resize(file.tellg());
			_STD cout << file.tellg() << '\n';
			file.seekg(0, _STD ios::beg);
			file.read(
				data.data(),
				static_cast<_STD streamsize>(data.size())
			);
#else
			FILE *file;
			errno_t r = fopen_s(&file, chars, "rb");
			gltfDebugPrintf("fopen_s returned %d", r);
			if (r != 0) {
				// Try appending the epic root directory
				_STD string full_path = (root / chars).string();
				r = fopen_s(&file, full_path.c_str(), "rb");
				gltfDebugPrintf("fopen_s with root appended returned %d", r);
				if (r != 0) {
					gltfDebugPrint("Failed to open buffer file.");
					return {};
				}
			}
			delete[] chars;
			_STD vector<char> data;
			fseek(file, 0, SEEK_END);
			data.resize(ftell(file));
			_STD cout << ftell(file) << '\n';
			fseek(file, 0, SEEK_SET);
			fread(
				data.data(),
				sizeof(char),
				data.size(),
				file
			);
			fclose(file);

#endif

			// validate buffer
			bool any_valid = false;
			for (auto const i : data) {
				if (i > 0) {any_valid = true; break;}
				//_STD cout << static_cast<i64>(i) << '\n';
			}
			assert(any_valid);
		
			return {_STD move(data)};
		}
		else {
			// TODO; this isnt common practice so just add it later.
		}
		return {};
	}

	buffer_view parse_buffer_view(ondemand::value &object) {
		buffer_view buffer_view{
			.buffer = 0,
			.length = 0,
			.offset = 0
		};
		for (simdjson_result kv : object.get_object()) {
			_STD string key;
			{
				char const *key_unsafe = kv.key().raw().value();
				_STD size_t size = 0;
				for (size = 0; size < 1024; size++) {
					if (key_unsafe[size] == '\0' || key_unsafe[size] == '\'' || key_unsafe[size] == '"') {
						break;
					}
				}
				key = _STD string(key_unsafe, size);
			}
			switch (hash(key)) {
				case hash("buffer"): {
					buffer_view.buffer = kv.value().get<id>().value();
					break;
				}
				case hash("byteLength"): {
					buffer_view.length = kv.value().get_uint64().value();
					break;
				}
				case hash("byteOffset"): {
					buffer_view.offset = kv.value().get_uint64().value();
					break;
				}
				case hash("target"): {
					buffer_view.target = static_cast<buffer_view_target>(kv.value().get<u16>().value());
					break;
				}
				default:
					break;
			}
		}
		return buffer_view;
	}

	mesh parse_meshes(ondemand::value &object) {
		simdjson_result<ondemand::value> name_obj = object["name"];
		_STD string const name(name_obj.get_string().value().data(), name_obj.get_string()->length());  // NOLINT(bugprone-suspicious-stringview-data-usage)

		auto weights_object = object["weights"];
		mesh mesh {
			.name = name,
			.primitives = {},
			.weights = weights_object.has_value() ? weights_object.get<_STD vector<float>>().value() : _STD vector<float>()
		};

		auto primitives_object = object["primitives"].get_array();
		mesh.primitives.reserve(primitives_object.count_elements().value());
		
		for (auto prims : primitives_object) {
			
			auto attribs = prims["attributes"].get_object();
			attributes attrib_array{};
			for (auto attrib_object : attribs) {

				auto const raw_name = attrib_object.key().value();
				//< essentially guaranteed that this is unescaped
				_STD size_t i;
				for (i = 0; i < 0xff; i++) {
					if (raw_name[i] == '"') break;
				}
				_STD string pure_name(raw_name.raw(), i);
				
				attrib_array.emplace_back(attribute{
					.name = pure_name,
					.accessor = attrib_object.value().get<i32>().value()
				});
			}
			mesh.primitives.push_back(primitive{
				.attributes = _STD move(attrib_array),
				.indices = prims["indices"].get<i32>(),
				.material = prims["material"].has_value() ? prims["material"].get<u32>().value() : 0u,
				.mode = prims["mode"].has_value() ?
					static_cast<primitive_mode>(prims["mode"].get_int64().value()) :
					primitive_mode::triangles,
			});
		}
		
		return mesh;
	}

	texture parse_texture(ondemand::value &object) {
		return {
			.sampler = object["sampler"].get<id>().value(),
			.source = object["source"].get<id>().value(),
		};
	}
}

static void my_png_err(png_structp png_ptr, char const *message) {
	std::cout << "png err: " << message << '\n';
}

static void my_png_warn(png_structp png_ptr, char const *message) {
	std::cout << "png warn: " << message << '\n';
}


static image parse_image(_STD filesystem::path &path, std::string &uri) {
		image image;
		//if (auto uri_object = object["uri"]; uri_object.has_value()) {
		{
			//_STD string uri (uri_object.get_string().value().data(), uri_object.get_string()->length());  // NOLINT(bugprone-suspicious-stringview-data-usage)
			// we don't really need anything else
#ifdef GLTF_THREADED_IMAGE_LOADING
			image.external_data = external_data.get_future();
			
			gltf::worker_threads_.emplace_back(_STD thread([&external_data, path, &image] {
				_STD cout << "STARTED" << '\n';
				auto const filepath = path / image.uri;
				int w, h = 0;
				int channels = 0;
				_STD string null_terminated(filepath.string().c_str(), filepath.string().length());
				_STD fstream file(filepath, _STD ios::binary);
				// get file size
				file.seekg(0, _STD ios::end);
				_STD size_t size = file.tellg();
				file.seekg(0, _STD ios::beg);
				_STD vector<u8> png_buffer(size);
				_STD cout << "CREATED RAW PNG BUFFER OF SIZE " << size << '\n';
				// read to vector
				file.read(reinterpret_cast<char*>(png_buffer.data()), size);
				stbi_uc const *buffer = stbi_load_from_memory(png_buffer.data(), size, &w, &h, &channels, STBI_rgb_alpha);
				_STD cout << "LOADED TO STBI FROM MEMORY TO POINTER " << (uintptr_t)buffer << '\n';
				_STD cout << filepath.string() << " is " << w << "x" << h << '\n';
				auto buf = _STD make_shared<_STD vector<u8>>(static_cast<_STD size_t>(w * h * channels));
				_STD copy_n(buffer, w * h * channels, buf->begin());
				stbi_image_free((void*)buffer);
				external_data.set_value(buf);
			}));
#else
			auto const filepath = path.parent_path() / uri;
			_STD string null_terminated(filepath.string().c_str(), filepath.string().length());
			int w, h;
#ifdef GLTF_USE_STD_FILESYSTEM
			_STD fstream file(null_terminated, _STD ios::binary);
			assert(file.is_open());
			// get file sizze
			file.seekg(0, _STD ios::end);
			_STD size_t size = file.tellg();
			file.seekg(0, _STD ios::beg);
			_STD vector<u8> png_buffer(size);
			_STD cout << "CREATED RAW PNG BUFFER OF SIZE " << size << '\n';
			// read to vector
			file.read(reinterpret_cast<char*>(png_buffer.data()), size);
			stbi_uc const *buffer = stbi_load_from_memory(png_buffer.data(), size, &w, &h, &channels, STBI_rgb_alpha);
			#else

			image.hash_value = ::hash(null_terminated);
			_STD string imageUid = ".local/img-cache/" + std::to_string(image.hash_value) + ".hltx";

			if (FILE *compressed_image = fopen(imageUid.c_str(), "rb"); compressed_image != nullptr) {
				u16 image_size[2]{};
				assert(fread_s(image_size, 4, 2, 2, compressed_image) == 2);
				glm::ivec2 true_size(static_cast<int>(image_size[0]), static_cast<int>(image_size[1]));
				
				u8 channels = 0;
				assert(fread_s(&channels, 1, 1, 1, compressed_image) == 1);

				_STD size_t remaining_data_size = (static_cast<_STD size_t>(true_size.x) * static_cast<_STD size_t>(true_size.y) * static_cast<_STD size_t>(channels));

				_STD vector<u8> compressed_pixels(remaining_data_size);
				size_t const read_result = fread(compressed_pixels.data(), 1, remaining_data_size, compressed_image);
				assert(read_result <= compressed_pixels.size());
				assert(fclose(compressed_image) == 0);

				compressed_pixels.resize(read_result);

				image.external_data = _STD make_shared<_STD vector<u8>>(compressed_pixels);

				gltf::image gltf_image = {
					.uri = uri,
					.channels = channels,
					.hash_value = hash(null_terminated),
					.compressed = true,
					.size = true_size,
					.external_data = _STD make_shared<_STD vector<u8>>(compressed_pixels)
				};

				return gltf_image;
			}
			else {
				FILE *f = fopen(null_terminated.c_str(), "rb");
				assert(f != nullptr && f != 0);

				png_structp png_ptr;
				png_infop info_ptr;

				png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, my_png_err, my_png_warn);
				info_ptr = png_create_info_struct(png_ptr);
				//png_set_benign_errors(png_ptr, 1);
				//png_set_crc_action(png_ptr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);
				png_init_io(png_ptr, f);

				png_read_info(png_ptr, info_ptr);

				auto const bit_depth = png_get_bit_depth(png_ptr, info_ptr);
				auto const channels = png_get_channels(png_ptr, info_ptr);
				w = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
				h = static_cast<int>(png_get_image_height(png_ptr, info_ptr));

				size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
				auto buffer = std::make_shared<std::vector<u8>>(static_cast<size_t>(h) * rowbytes);
				u8 *buffer_data = buffer->data();
				
				std::vector<png_bytep> row_pointers(h);
				for (int i = 0; i < h; i++)
					row_pointers[i] = buffer_data + i * rowbytes;
				png_read_image(png_ptr, row_pointers.data());
			
#endif
			
				gltf::image gltf_image = {
					.uri = uri,
					.channels = channels,
					.hash_value = hash(null_terminated),
					.compressed = false,
					.size = glm::ivec2(w,h),
					.external_data = buffer
				};

				assert(_CrtCheckMemory());
				png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
				int ret = fclose(f);
				assert(ret == 0);
			
#endif
				return gltf_image;
			}
		}
	/*
		else {
			image.mimeType = object["mimeType"].get<_STD string>();
			image.bufferView = object["bufferView"].get<id>();
			return image;
		} 
#ifndef GLTF_IGNORE_NAMES
		if (simdjson_result<ondemand::value> name_object = object["name"]; name_object.has_value())
			image.name = name_object.get<_STD string>(); // optional
#endif
*/
		image.size = glm::ivec2(-1, -1); 
		return image;
	}

namespace  {

	sampler parse_sampler(ondemand::value &object) {
		sampler sampler{};
		if (auto mag_filter_object = object["magFilter"]; mag_filter_object.has_value())
			sampler.mag_filter = (gl::TextureMagFilter)mag_filter_object.get<gl::enum_t>().value();
		if (auto min_filter_object = object["minFilter"]; min_filter_object.has_value())
			sampler.min_filter = (gl::TextureMinFilter)min_filter_object.get<gl::enum_t>().value();
		if (auto wrap_s_object = object["wrapS"]; wrap_s_object.has_value())
			sampler.wrap_s_mode = (gl::TextureWrapMode)wrap_s_object.get<gl::enum_t>().value();
		if (auto wrap_t_object = object["wrapT"]; wrap_t_object.has_value())
			sampler.wrap_t_mode = (gl::TextureWrapMode)wrap_t_object.get<gl::enum_t>().value();
		return sampler;
	}

	material parse_material(ondemand::value &object) {
		material material{};

		for (simdjson_result elem : object.get_object()) {
			_STD string key;
			{
				char const *key_unsafe = elem.key().raw().value();
				_STD size_t size = 0;
				for (size = 0; size < 1024; size++) {
					if (key_unsafe[size] == '\0' || key_unsafe[size] == '\'' || key_unsafe[size] == '"') {
						break;
					}
				}
				key = _STD string(key_unsafe, size);
			}

			material.emissive_texture = {};
			
			switch (hash(key)) {
				case hash("emissiveTexture"): {
					ondemand::object elemVal = elem.value().get_object().value();
					material.emissive_texture.index = elemVal["index"].get<id>();
					break;
				}
				case hash("emissiveFactor"): {
					material.emissive_factor = glm::vec4(0.0f);
					_STD size_t i = 0;
					for (simdjson_result value : elem.value().get_array())
						material.emissive_factor[i++] = value.get<number>().value();
					break;
				}
				case hash("normalTexture"): {
					ondemand::object elemVal = elem.value().get_object().value();
					material.normal_texture.index = elemVal["index"].get<id>();

					if (simdjson_result<ondemand::value> scale = elemVal["scale"]; scale.has_value())
						material.normal_texture.scale = scale.get<number>().value();

					if (simdjson_result<ondemand::value> texcoord = elemVal["texCoord"]; texcoord.has_value())
						material.normal_texture.scale = texcoord.get<id>().value();

					break;
				}
				case hash("occlusionTexture"): {
					material.occlusion_texture = {
						.index = elem.value()["index"].get<id>()
					};
					break;
				}
				case hash("pbrMetallicRoughness"): {
					for (auto pbr = elem.value().get_object(); auto pbr_elem : pbr) {
						_STD string pbr_key;
						{
							char const *pbr_key_unsafe = pbr_elem.key().raw().value();
							_STD size_t size = 0;
							for (size = 0; size < 1024; size++)
								if (pbr_key_unsafe[size] == '\0' || pbr_key_unsafe[size] == '\'' || pbr_key_unsafe[size] == '"')
									break;
							pbr_key = _STD string(pbr_key_unsafe, size);
						}
						switch (hash(pbr_key)) {
							case hash("baseColorTexture"): {
								material.pbr_metallic_roughness.base_color_texture = {
									.index = pbr_elem.value()["index"].get<id>(),
									.tex_coord = 0,
									.scale = 1.0
								};
								break;
							}

							case hash("baseColorFactor"): {
								material.pbr_metallic_roughness.base_color_factor = {};
								_STD size_t iterator = 0;
								for (simdjson_result value : pbr_elem.value().get_array())
									material.pbr_metallic_roughness.base_color_factor[iterator++] = value.get<number>().value();
								break;
							}
					
							case hash("metallicRoughnessTexture"): {
								material.pbr_metallic_roughness.metallic_roughness_texture = {
									.index = pbr_elem.value()["index"].get<id>(),
									.tex_coord = 0,
									.scale = 1.0
								};
								break;
							}

							case hash("metallicFactor"): {
								material.pbr_metallic_roughness.metallic_factor = pbr_elem.value().get<number>().value();
								break;
							}
							case hash("roughnessFactor"): {
								material.pbr_metallic_roughness.roughness_factor = pbr_elem.value().get<number>().value();
								break;
							}
							default:
								break;
						}
					}
					break;
				}
				case hash("name"):
					material.name = elem.value().get<_STD string>();
					break;
				default:
					break;
			}
		}
		return material;
	}

	node parse_node(ondemand::value &object) {
		node node{};

		if (auto children_object = object["children"]; children_object.has_value())
			for (auto children_arr = children_object.get_array();
				auto child : children_arr)
				node.children.push_back(child.get<id>().value());

		if (auto mesh_object = object["mesh"]; mesh_object.has_value())
			node.mesh = mesh_object.value().get<id>();

		if (auto name_object = object["name"]; name_object.has_value())
			node.name = name_object.get<_STD string>();

		if (auto skin_object = object["skin"]; skin_object.has_value())
			node.skin = skin_object.get<id>();

		auto matrix_object = object["matrix"];
		if (matrix_object.has_value()) {
			
		}
		else {
			
			i32 i = 0;
			if (simdjson_result<ondemand::value> rotation_object = object["rotation"]; 
				rotation_object.has_value()) {
				node.has_transform = true;
				simdjson_result<ondemand::array> rarr = rotation_object.get_array();
				for (i = 0; auto v : rarr) {
					if (i > 3) break;
					node.rotation[i] = v.get<f32>().value();
					++i;
				}
			}
			else {
				node.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			}

			if (simdjson_result<ondemand::value> scale_object = object["scale"]; 
				scale_object.has_value()) {
				node.has_transform = true;
				simdjson_result<ondemand::array> sarr = scale_object.get_array();
				for (i = 0; auto v : sarr) {
					if (i > 2) break;
					node.scale[i] = v.get<f32>().value();
					++i;
				}
			}
			else {
				node.scale = glm::vec3(1.0f);
			}

			if (simdjson_result<ondemand::value> translation_object = object["translation"]; 
				translation_object.has_value()) {
				node.has_transform = true;
				simdjson_result<ondemand::array> tarr = translation_object.get_array();
				for (i = 0; auto v : tarr) {
					if (i > 2) break;
					node.translation[i] = v.get<f32>().value();
					++i;
				}
			}
		}
		
		return node;
	}

	skin parse_skin(ondemand::value &object) {
		skin skin{};

		if (auto inverse_bind_matrices_object = object["inverseBindMatrices"]; inverse_bind_matrices_object.has_value())
			skin.inverseBindMatrices = inverse_bind_matrices_object.get<id>();

		if (auto joints_value = object["joints"]; joints_value.has_value()) {
			auto joints_array = joints_value.get_array();
			skin.joints.reserve(joints_array.count_elements());
			for (auto joint : joints_array)
				skin.joints.push_back(joint.get<id>());
		}

		if (auto name_value = object["name"]; name_value.has_value())
			skin.name = name_value.get<_STD string>();
		
		return skin;
	}

	scene parse_scene(ondemand::value &object) {
		scene scene{};

		// name
		if (auto name_object = object["name"]; name_object.has_value())
			scene.name = name_object.get<_STD string>();

		auto nodes = object["nodes"].get_array();
		for (auto node : nodes)
			scene.nodes.push_back(node.get_int64().value());

		return scene;
	}
}

data gltf::parse(_STD string const& file_path, padded_string &&file) {
	data gltf_data;
	simdjson_result const json = _STD move(file);

	// Save the base directory of the file, this is applied to relative directories
	//_STD cout << "PP: " << file_path << '\n';
	_STD filesystem::path path(file_path);

	gltf_data.path = path;

	auto load_mesh_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;

		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		ondemand::value meshes_obj = obj["meshes"].value();

		for (
			simdjson_result mesh_obj : meshes_obj
			) {
			assert(mesh_obj.has_value());
			mesh mesh = parse_meshes(mesh_obj.value());
			gltf_data.meshes.emplace_back(_STD move(mesh));
		}
	});

	auto load_images_promise = std::async([&gltf_data, &json, &path]() {
		ondemand::parser parser;

		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();

		ondemand::value images_obj = obj["images"].value();
		std::vector<std::future<image>> images_promise(gltf_data.images.size());
		images images(images_obj.count_elements());
		for (
			simdjson_result image_obj : images_obj
			) {
			assert(image_obj.has_value());
			//images_promise.push_back(std::async([](_STD filesystem::path &path__, ondemand::value &object__) { return parse_image(path__, object__); }, path, image_obj.value()));
			auto &image_object_valued = image_obj.value();
			_STD string uri (image_object_valued["uri"].get_string().value().data(), image_object_valued["uri"].get_string()->length());  // NOLINT(bugprone-suspicious-stringview-data-usage)
			images_promise.push_back(std::async([&path, uri](){ _STD string uri2 = uri; return parse_image(path, uri2); }));
		}

		// gltfDebugPrintf("GLTF File has %llu images\n", gltf_data.images.size());

		for (auto &i : images_promise) {
			gltf_data.images.push_back(i.get());
		}
	});

	auto load_textures_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;

		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		ondemand::value textures_obj = obj["textures"].value();
		textures textures(textures_obj.count_elements());

		for (
			simdjson_result texture_obj : textures_obj
		) {
			assert(texture_obj.has_value());
			texture texture = parse_texture(texture_obj.value());
			gltf_data.textures.emplace_back(texture);
		}

		// gltfDebugPrintf("GLTF File has %llu textures\n", gltf_data.textures.size());
		// return textures;
	});

	auto load_samplers_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;

		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		ondemand::value samplers_obj = obj["samplers"].value();
		//samplers samplers(samplers_obj.count_elements());

		for (
			simdjson_result sampler_obj : samplers_obj
		) {
			assert(sampler_obj.has_value());
			sampler sampler = parse_sampler(sampler_obj.value());
			gltf_data.samplers.emplace_back(sampler);
		}

		// gltfDebugPrintf("GLTF File has %llu samplers\n", gltf_data.samplers.size());
		// return samplers;
	});

	auto load_materials_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;

		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		ondemand::value materials_obj = obj["materials"].value();

		for (simdjson_result mat_obj : materials_obj) {
			assert(mat_obj.has_value());
			material mat = parse_material(mat_obj.value());
			gltf_data.materials.emplace_back(_STD move(mat));
		}
	});

	auto load_nodes_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;
		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		ondemand::value nodes_obj = obj["nodes"].value();
		std::vector<node> nodes(nodes_obj.count_elements());
		for (simdjson_result node_obj : nodes_obj) {
			assert(node_obj.has_value());
			node node = parse_node(node_obj.value());
			gltf_data.nodes.push_back(node);
		}
	});

	auto load_scenes_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;
		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		ondemand::value scenes_obj = obj["scenes"].value();
		for (simdjson_result scene_obj : scenes_obj) {
			assert(scene_obj.has_value());
			scene scene = parse_scene(scene_obj.value());
			gltf_data.scenes.push_back(scene);
		}
		ondemand::value scene_id = obj["scene"].value();
		gltf_data.scene = scene_id.get<id>();
	});

	auto load_skins_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;
		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		auto skins_result_obj = obj["skins"];
		if (!skins_result_obj.has_value())
			return;

		ondemand::value skins_obj = skins_result_obj.value();
		//gltf::skins sk(skins_obj.count_elements());
		for (simdjson_result skin_obj : skins_obj) {
			assert(skin_obj.has_value());
			skin skin = parse_skin(skin_obj.value());
			gltf_data.skins.push_back(skin);
			//sk.emplace_back(skin);
		}
	});
	
	auto load_accessors_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;
		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		ondemand::value accessors = obj["accessors"].value();
		for (
			simdjson_result accessor : accessors
		) {
			assert(accessor.has_value());
			gltf::accessor a = parse_accessor(accessor.value());
			//_STD cout << "buffer view " << a.bufferView() << '\n' << "type " << to_string(a.type()) << '\n' << "comp type " << to_string(a.componentType()) << '\n' << "count " << a.count() << '\n' << '\n';
			gltf_data.accessors.push_back(a);
		}
	});

	auto load_buffer_views_promise = std::async([&gltf_data, &json]() {
		ondemand::parser parser;
		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		ondemand::array buffer_views = obj["bufferViews"].get_array().value();
		for (
			simdjson_result buffer_view : buffer_views
		) {
			assert(buffer_view.has_value());
			gltf::buffer_view view = parse_buffer_view(buffer_view.value());
			gltf_data.buffer_views.emplace_back(view);
		}
	});

	auto load_buffers_promise = std::async([&gltf_data, &json, path]() {
		ondemand::parser parser;
		ondemand::document doc = parser.iterate(json);
		auto obj = doc.get_object();
		for (
			ondemand::value buffers = obj["buffers"].value();
			simdjson_result buffer : buffers
			) {
			assert(buffer.has_value());
			auto root_directory = path.parent_path();
			gltf::buffer buf = parse_buffer(root_directory, buffer.value());
			gltf_data.buffers.emplace_back(_STD move(buf));
		}

		// print num of buffers
		// gltfDebugPrintf("GLTF File has %llu buffer(s)", gltf_data.buffers.size());
	});

	load_mesh_promise.get();
	load_images_promise.get();
	load_textures_promise.get();
	load_samplers_promise.get();
	load_materials_promise.get();
	load_nodes_promise.get();
	load_scenes_promise.get();
	load_skins_promise.get();
	load_accessors_promise.get();
	load_buffer_views_promise.get();
	load_buffers_promise.get();
	
	gltfDebugPrint("-- GLTF DUMP --");
	gltfDebugPrintf("Mesh count: %llu", gltf_data.meshes.size());
	gltfDebugPrintf("Material count: %llu", gltf_data.materials.size());
	gltfDebugPrintf("Node count: %llu", gltf_data.nodes.size());
	gltfDebugPrintf("Scene count: %llu", gltf_data.scenes.size());
	gltfDebugPrintf("Skin count: %llu", gltf_data.skins.size());
	gltfDebugPrintf("Accessor count: %llu", gltf_data.accessors.size());
	gltfDebugPrintf("Buffer view count: %llu", gltf_data.buffer_views.size());
	gltfDebugPrintf("Buffer count: %llu", gltf_data.buffers.size());
	gltfDebugPrintf("Texture count: %llu", gltf_data.textures.size());

	return gltf_data;
}