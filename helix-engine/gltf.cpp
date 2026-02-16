#include "gltf.h"
#include "simdjson/simdjson.h"

#include <cassert>
#include <fstream>

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
	
	std::fstream file(uri_.value(), std::ios::binary);

	file.seekg(0, std::ios::end);
	data_.resize(file.tellg());
	file.seekg(0, std::ios::beg);

	file.read(
		reinterpret_cast<char *>(data_.data()),
		static_cast<std::streamsize>(data_.size())
	);
}

CGltfBuffer::CGltfBuffer(std::vector<char> &&data) : data_(std::move(data)) {}
CGltfBufferView::CGltfBufferView(std::shared_ptr<CGltfBuffer> const &buffer, std::uint32_t const length, std::uint32_t const offset, ETarget const target): buffer_(buffer), length_(length), offset_(offset), target_(target) {}

//
//
// MAIN ACTUAL PARSING IS BELOW!!
//
//
using namespace simdjson;

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

		return std::move(a);
	}

	CGltfBuffer parse_buffer(ondemand::value &object) {
		if (auto uri = object["uri"]; uri.has_value()) {
			gltfDebugPrint("Buffer contains a uri, not inline data.");
			auto const text = uri.get_string().value();
			auto const chars = new char[text.length() + 1];
			std::memset(chars, 0, text.length() + 1);
			text.copy(chars, text.length());
			gltfDebugPrintf("Buffer's URI is \"%s\"", chars);
			std::fstream file(chars, std::ios::in | std::ios::binary);
			gltfDebugPrintf("file.is_open() = %s", file.is_open() ? "TRUE" : "FALSE");
			delete[] chars;
			assert(file.is_open());
		
			std::vector<char> data;
			file.seekg(0, std::ios::end);
			data.resize(file.tellg());
			std::cout << file.tellg() << '\n';
			file.seekg(0, std::ios::beg);
			file.read(
				data.data(),
				static_cast<std::streamsize>(data.size())
			);

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
}

GltfData_t gltf::parse(padded_string &&file) {
	GltfData_t gltf_data;
	ondemand::parser parser;
	simdjson_result const json = std::move(file);

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
		CGltfBuffer buf = parse_buffer(buffer.value());
		gltf_data.buffers.emplace_back(std::move(buf));
	}

	// print num of buffers
	gltfDebugPrintf("GLTF File has %llu buffer(s)", gltf_data.buffers.size());
	
	return gltf_data;
}