// ReSharper disable CppCStyleCast
#include "gltf.h"
#include "simdjson/simdjson.h"

#include <cassert>
#include <fstream>
#include "stb/stb_image.h"

#include "types.hpp"


void CGltfProperty::setName(std::string const &p_name) { name_ = p_name; }
std::string const & CGltfProperty::name() const { return name_; }

// Gltf Accessor

CGltfAccessor::CGltfAccessor() = default;

void CGltfAccessor::setComponentType(GltfComponentType_e const p_type) { component_type_ = p_type; }
GltfComponentType_e CGltfAccessor::componentType() const { return component_type_; }

void CGltfAccessor::setType(GltfType_e const p_type) { type_ = p_type; }
GltfType_e CGltfAccessor::type() const { return type_; }

void CGltfAccessor::setBufferView(gltf::id const p_bufferViewIndex) { buffer_view_ = p_bufferViewIndex; }
gltf::id CGltfAccessor::bufferView() const { return buffer_view_; }

void CGltfAccessor::setOffset(std::uint32_t const p_offset) { offset_ = p_offset; }
std::uint32_t CGltfAccessor::offset() const { return offset_; }

void CGltfAccessor::setCount(std::uint32_t const p_count) { count_ = p_count; }
std::uint32_t CGltfAccessor::count() const { return count_; }

void CGltfAccessor::setMax(std::array<GLTF_NUMBER, 16> const &p_max) { max_ = p_max; }
void CGltfAccessor::setMaxComponent(std::size_t const p_index, GLTF_NUMBER const p_value) { max_[p_index] = p_value; }
std::array<GLTF_NUMBER, 16> const & CGltfAccessor::max() const { return max_; }

void CGltfAccessor::setMin(std::array<GLTF_NUMBER, 16> const &p_min) { min_ = p_min; }
void CGltfAccessor::setMinComponent(std::size_t const p_index, GLTF_NUMBER const p_value) { min_[p_index] = p_value; }
std::array<GLTF_NUMBER, 16> const & CGltfAccessor::min() const { return min_; }

CGltfBuffer::CGltfBuffer(std::string const& uri, std::string const& name)
	: uri_(uri), name_(name) {
	
#ifdef GLTF_USE_STD_FILESYSTEM
	std::fstream file(uri_.value(), std::ios::binary);

	file.seekg(0, std::ios::end);
	std::cout << file.tellg() << '\n';
	data_.resize(file.tellg());
	file.seekg(0, std::ios::beg);

	file.read(
		reinterpret_cast<char *>(data_.data()),
		static_cast<std::streamsize>(data_.size())
	);
#else
	std::cout << "GltfBuffer: Loading buffer from URI: " << uri << '\n';
	FILE *file;
	errno_t r = fopen_s(&file, uri.c_str(), "rb");
	assert(r == 0);

	fseek(file, 0, SEEK_END);
	std::cout << ftell(file) << '\n';
	data_.resize(ftell(file));
	fseek(file, 0, SEEK_SET);
	fread(
		data_.data(),
		sizeof(char),
		data_.size(),
		file
	);
	fclose(file);
#endif
}

CGltfBuffer::CGltfBuffer(std::vector<char> &&data) : data_(std::move(data)) {}
CGltfBufferView::CGltfBufferView(std::shared_ptr<CGltfBuffer> const &buffer, std::uint32_t const length, std::uint32_t const offset, ETarget const target): buffer_(buffer), length_(length), offset_(offset), target_(target) {}

//
//
// MAIN ACTUAL PARSING IS BELOW!!
//
//
using namespace simdjson;

std::vector<std::thread> gltf_worker_threads_;

namespace {
	CGltfAccessor parse_accessor(ondemand::value &accessor) {
		CGltfAccessor a;
		simdjson_result<ondemand::value> bv = accessor["bufferView"];
		simdjson_result<ondemand::value> ct = accessor["componentType"];
		simdjson_result<ondemand::value> t = accessor["type"];
		simdjson_result<ondemand::value> mn = accessor["min"];
		simdjson_result<ondemand::value> max = accessor["max"];
		simdjson_result<ondemand::value> cn = accessor["count"];

		gltfDebugPrintf("[Accessor] bufferView is valid %s", bv.has_value() ? "true" : "false");
		gltfDebugPrintf("[Accessor] componentType: %s", ct.has_value() ? "true" : "false");
		gltfDebugPrintf("[Accessor] type: %s", t.has_value() ? "true" : "false");
		

		if (bv.has_value()) a.setBufferView(static_cast<gltf::id>(bv.get_int64().value()));
		if (ct.has_value()) {
			switch(auto const ctv = ct.get_int64().value()) {
				case 5126:
					a.setComponentType(GltfComponentType_e::FLOAT);
					break;
				case 5125:
					a.setComponentType(GltfComponentType_e::USHORT);
					break;
				default: break;
			}
		}

		if (t.has_value()) {
			if (std::string_view const text = t.get_string().value(); text.length() == 6) {
				a.setType(GltfType_e::SCALAR); // scalar is 6 letters lol, fun little way to optimize
			}
			else {
				char const initial = text[0];
				char const number = text[3];
				switch (initial) {
					case 'V': {
						switch (number) {
							case '2': {
								a.setType(GltfType_e::VEC2);
								break;
							}
							case '3': {
								a.setType(GltfType_e::VEC3);
								break;
							}
							case '4': {
								a.setType(GltfType_e::VEC4);
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
								a.setType(GltfType_e::MAT2);
								break;
							}
							case '3': {
								a.setType(GltfType_e::MAT3);
								break;
							}
							case '4': {
								a.setType(GltfType_e::MAT4);
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
			a.setCount(static_cast<std::uint32_t>(cn.get_int64().value()));
		}

		return (a);
	}

	CGltfBuffer parse_buffer(std::filesystem::path &root, ondemand::value &object) {
		if (auto uri = object["uri"]; uri.has_value()) {
			gltfDebugPrint("Buffer contains a uri, not inline data.");
			auto const text = uri.get_string().value();
			auto const chars = new char[text.length() + 1];
			std::memset(chars, 0, text.length() + 1);
			text.copy(chars, text.length());
			gltfDebugPrintf("Buffer's URI is \"%s\"", chars);
#ifdef GLTF_USE_STD_FILESYSTEM
			std::fstream file(chars, std::ios::in | std::ios::binary);
			gltfDebugPrintf("file.is_open() = %s", file.is_open() ? "TRUE" : "FALSE");

			if (!file.is_open()) {
				// Try appending the epic root directory
				file = std::fstream(root / chars, std::ios::in | std::ios::binary);
			}
			delete[] chars;
		
			std::vector<char> data;
			file.seekg(0, std::ios::end);
			data.resize(file.tellg());
			std::cout << file.tellg() << '\n';
			file.seekg(0, std::ios::beg);
			file.read(
				data.data(),
				static_cast<std::streamsize>(data.size())
			);
#else
			FILE *file;
			errno_t r = fopen_s(&file, chars, "rb");
			gltfDebugPrintf("fopen_s returned %d", r);
			if (r != 0) {
				// Try appending the epic root directory
				std::string full_path = (root / chars).string();
				r = fopen_s(&file, full_path.c_str(), "rb");
				gltfDebugPrintf("fopen_s with root appended returned %d", r);
				if (r != 0) {
					gltfDebugPrint("Failed to open buffer file.");
					return {};
				}
			}
			delete[] chars;
			std::vector<char> data;
			fseek(file, 0, SEEK_END);
			data.resize(ftell(file));
			std::cout << ftell(file) << '\n';
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
				//std::cout << static_cast<i64>(i) << '\n';
			}
			assert(any_valid);
		
			return {std::move(data)};
		}
		else {
			// TODO; this isnt common practice so just add it later.
		}
		return {};
	}

	GltfBufferView_t parse_buffer_view(ondemand::value &object) {
		gltf::id const buffer = object["buffer"].get<gltf::id>();
		u32 const byte_length = object["byteLength"].get<u32>();
		u32 const byte_offset = object["byteOffset"].get<u32>();
		auto const target = static_cast<GltfBufferViewTarget_e>(object["target"].get_int64().value());
		
		return {
			.buffer = buffer,
			.length = byte_length,
			.offset = byte_offset,
			.target = target
		};
	}
	

	GltfMesh_t parse_meshes(ondemand::value &object) {
		simdjson_result<ondemand::value> name_obj = object["name"];
		std::string const name(name_obj.get_string().value().data(), name_obj.get_string()->length());  // NOLINT(bugprone-suspicious-stringview-data-usage)

		auto weights_object = object["weights"];
		GltfMesh_t mesh {
			.name = name,
			.primitives = {},
			.weights = weights_object.has_value() ? weights_object.get<std::vector<float>>().value() : std::vector<float>()
		};
		
		for (auto prims : object["primitives"]) {
			
			auto attribs = prims["attributes"].get_object();
			GltfMeshPrimitiveAttributes attrib_array{};
			for (auto attrib_object : attribs) {

				auto const raw_name = attrib_object.key().value();
				//< essentially guaranteed that this is unescaped
				std::size_t i;
				for (i = 0; i < 0xff; i++) {
					if (raw_name[i] == '"') break;
				}
				std::string name(raw_name.raw(), i);
				
				GltfMeshPrimitiveAttrib_t attrib{
					.name = name,
					.accessor = attrib_object.value().get<i32>().value()
				};
				attrib_array.emplace_back(std::move(attrib));
			}
			
			GltfMeshPrimitive_t primitive{
				.attributes = std::move(attrib_array),
				.indices = prims["indices"].get<i32>(),
				.mode = prims["mode"].has_value() ?
					static_cast<GltfMeshPrimitiveMode_e>(prims["mode"].get_int64().value()) :
					GltfMeshPrimitiveMode_e::TRIANGLES,
			};

			
			
			mesh.primitives.emplace_back(std::move(primitive));
		}
		
		return mesh;
	}

	GltfTexture_t parse_texture(ondemand::value &object) {
		return {
			.sampler = object["sampler"].get<gltf::id>().value(),
			.source = object["source"].get<gltf::id>().value(),
		};
	}

	void parse_image(GltfImage_t &image, std::filesystem::path &path, ondemand::value &object) {

		if (auto uri_object = object["uri"]; uri_object.has_value()) {
			image.uri = std::string(uri_object.get_string().value().data(), uri_object.get_string()->length());  // NOLINT(bugprone-suspicious-stringview-data-usage)
			// we don't really need anything else
			std::promise<std::shared_ptr<std::vector<u8>>> external_data;
			#ifdef GLTF_THREADED_IMAGE_LOADING
			image.external_data = external_data.get_future();
			
			gltf_worker_threads_.emplace_back(std::thread([&external_data, path, &image] {
				std::cout << "STARTED" << '\n';
				auto const filepath = path / image.uri;
				int w, h = 0;
				int channels = 0;
				std::string null_terminated(filepath.string().c_str(), filepath.string().length());
				std::fstream file(filepath, std::ios::binary);
				// get file sizze
				file.seekg(0, std::ios::end);
				std::size_t size = file.tellg();
				file.seekg(0, std::ios::beg);
				std::vector<u8> png_buffer(size);
				std::cout << "CREATED RAW PNG BUFFER OF SIZE " << size << '\n';
				// read to vector
				file.read(reinterpret_cast<char*>(png_buffer.data()), size);
				stbi_uc const *buffer = stbi_load_from_memory(png_buffer.data(), size, &w, &h, &channels, STBI_rgb_alpha);
				std::cout << "LOADED TO STBI FROM MEMORY TO POINTER " << (uintptr_t)buffer << '\n';
				std::cout << filepath.string() << " is " << w << "x" << h << '\n';
				auto buf = std::make_shared<std::vector<u8>>(static_cast<std::size_t>(w * h * channels));
				std::copy_n(buffer, w * h * channels, buf->begin());
				stbi_image_free((void*)buffer);
				external_data.set_value(buf);
			}));
			#else
			auto const filepath = path.parent_path() / image.uri;
			int w, h = 0;
			int channels = 0;
			std::string null_terminated(filepath.string().c_str(), filepath.string().length());
			std::cout << null_terminated << '\n';
			#ifdef GLTF_USE_STD_FILESYSTEM
			std::fstream file(null_terminated, std::ios::binary);
			assert(file.is_open());
			// get file sizze
			file.seekg(0, std::ios::end);
			std::size_t size = file.tellg();
			file.seekg(0, std::ios::beg);
			std::vector<u8> png_buffer(size);
			std::cout << "CREATED RAW PNG BUFFER OF SIZE " << size << '\n';
			// read to vector
			file.read(reinterpret_cast<char*>(png_buffer.data()), size);
			stbi_uc const *buffer = stbi_load_from_memory(png_buffer.data(), size, &w, &h, &channels, STBI_rgb_alpha);
			#else
			stbi_uc *buffer = stbi_load(null_terminated.c_str(), &w, &h, &channels, STBI_rgb_alpha);

			std::cout << channels << '\n';
			#endif
			std::cout << "LOADED TO STBI FROM MEMORY TO POINTER " << (uintptr_t)buffer << '\n';
			std::cout << filepath.string() << " is " << w << "x" << h << '\n';
			image.external_data = std::vector<u8>(static_cast<std::size_t>(w * h * channels));
			std::copy_n(buffer, w * h * channels, image.external_data.begin());
			stbi_image_free((void*)buffer);
				
			#endif
		}
		else {
			image.mimeType = object["mimeType"].get<std::string>();
			image.bufferView = object["bufferView"].get<gltf::id>();
		}

#ifndef GLTF_IGNORE_NAMEWS
		if (simdjson_result<ondemand::value> name_object = object["name"]; name_object.has_value())
			image.name = name_object.get<std::string>(); // optional
#endif
		
	}

	GltfSampler_t parse_sampler(ondemand::value &object) {
		GltfSampler_t sampler{};
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
}

GltfData_t gltf::parse(std::string const& file_path, padded_string &&file) {
	GltfData_t gltf_data;
	ondemand::parser parser;
	simdjson_result const json = std::move(file);

	// Save the base directory of the file, this is applied to relative directories
	std::cout << "PP: " << file_path << '\n';
	std::filesystem::path path(file_path);

	gltf_data.path = path;

	ondemand::document doc = parser.iterate(json);
	auto obj = doc.get_object();

	for (
		ondemand::value meshes_obj = obj["meshes"].value();
		simdjson_result mesh_obj : meshes_obj
	) {
		assert(mesh_obj.has_value());
		GltfMesh_t mesh = parse_meshes(mesh_obj.value());
		gltf_data.meshes.emplace_back(std::move(mesh));
	}
	
	gltfDebugPrintf("GLTF File has %llu meshes\n", gltf_data.meshes.size());

	for (
		ondemand::value images_obj = obj["images"].value();
		simdjson_result image_obj : images_obj
	) {
		assert(image_obj.has_value());
		GltfImage_t image;
		parse_image(image, path, image_obj.value());
		gltf_data.images.emplace_back(std::move(image));
	}

	gltfDebugPrintf("GLTF File has %llu images\n", gltf_data.images.size());

	for (
		ondemand::value textures_obj = obj["textures"].value();
		simdjson_result texture_obj : textures_obj
	) {
		assert(texture_obj.has_value());
		GltfTexture_t texture = parse_texture(texture_obj.value());
		gltf_data.textures.emplace_back(texture);
	}

	gltfDebugPrintf("GLTF File has %llu textures\n", gltf_data.textures.size());

	for (
		ondemand::value samplers_obj = obj["samplers"].value();
		simdjson_result sampler_obj : samplers_obj
	) {
		assert(sampler_obj.has_value());
		GltfSampler_t sampler = parse_sampler(sampler_obj.value());
		gltf_data.samplers.emplace_back(sampler);
	}

	gltfDebugPrintf("GLTF File has %llu samplers\n", gltf_data.samplers.size());
	
	for (
		ondemand::value accessors = obj["accessors"].value();
		simdjson_result accessor : accessors
	) {
		assert(accessor.has_value());
		CGltfAccessor a = parse_accessor(accessor.value());
		//std::cout << "buffer view " << a.bufferView() << '\n' << "type " << to_string(a.type()) << '\n' << "comp type " << to_string(a.componentType()) << '\n' << "count " << a.count() << '\n' << '\n';
		gltf_data.accessors.emplace_back(std::move(a));
	}

	gltfDebugPrintf("GLTF File has %llu accessor(s)", gltf_data.accessors.size());
	
	for (
		ondemand::value buffer_views = obj["bufferViews"].value();
		simdjson_result buffer_view : buffer_views
	) {
		assert(buffer_view.has_value());
		GltfBufferView_t view = parse_buffer_view(buffer_view.value());
		gltf_data.buffer_views.emplace_back(view);
	}

	gltfDebugPrintf("GLTF File has %llu buffer view(s)", gltf_data.buffer_views.size());

	for (
		ondemand::value buffers = obj["buffers"].value();
		simdjson_result buffer : buffers
	) {
		assert(buffer.has_value());
		auto root_directory = path.parent_path();
		CGltfBuffer buf = parse_buffer(root_directory, buffer.value());
		gltf_data.buffers.emplace_back(std::move(buf));
	}

	// print num of buffers
	gltfDebugPrintf("GLTF File has %llu buffer(s)", gltf_data.buffers.size());
	
	return gltf_data;
}