#include "gltf.h"
#include "simdjson/simdjson.h"

#include <cassert>
#include <fstream>

#include "types.hpp"

void CGltfProperty::setName(std::string const &p_name) { name_ = p_name; }
std::string const & CGltfProperty::name() const { return name_; }

// Gltf Accessor

CGltfAccessor::CGltfAccessor() = default;

void CGltfAccessor::setComponentType(EGltfComponentType const p_type) { component_type_ = p_type; }
EGltfComponentType CGltfAccessor::componentType() const { return component_type_; }

void CGltfAccessor::setType(EGltfType const p_type) { type_ = p_type; }
EGltfType CGltfAccessor::type() const { return type_; }

void CGltfAccessor::setBufferView(gltf::id const p_bufferViewIndex) { buffer_view_ = p_bufferViewIndex; }
gltf::id CGltfAccessor::bufferView() const { return buffer_view_; }

void CGltfAccessor::setOffset(std::uint32_t const p_offset) { offset_ = p_offset; }
std::uint32_t CGltfAccessor::offset() const { return offset_; }

void CGltfAccessor::setCount(std::uint32_t const p_count) { count_ = p_count; }
std::uint32_t CGltfAccessor::count() const { return count_; }

void CGltfAccessor::setMax(std::array<gltf::number, 16> const &p_max) { max_ = p_max; }
void CGltfAccessor::setMaxComponent(std::size_t const p_index, gltf::number const p_value) { max_[p_index] = p_value; }
std::array<gltf::number, 16> const & CGltfAccessor::max() const { return max_; }

void CGltfAccessor::setMin(std::array<gltf::number, 16> const &p_min) { min_ = p_min; }
void CGltfAccessor::setMinComponent(std::size_t const p_index, gltf::number const p_value) { min_[p_index] = p_value; }
std::array<gltf::number, 16> const & CGltfAccessor::min() const { return min_; }

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
	void parseAccessors(CGltfAccessor &a, ondemand::value &accessor) {
		auto bv = accessor["bufferView"];
		auto ct = accessor["componentType"];
		auto t = accessor["type"];
		auto mn = accessor["min"];
		auto max = accessor["max"];
		auto cn = accessor["count"];

		if (!bv.is_null()) a.setBufferView(static_cast<u32>(bv.get_int64().value()));
		if (!ct.is_null()) {
			auto const ctv = ct.get_int64().value();
			switch(ctv) {
				case 5126:
					a.setComponentType(EGltfComponentType::FLOAT);
					break;
				case 5125:
					a.setComponentType(EGltfComponentType::USHORT);
					break;
				default: break;
			}
		}

		if (!t.is_null()) {
			if (auto const text = t.get_string().value(); text.length() == 6) {
				a.setType(EGltfType::SCALAR); // scalar is 6 letters lol, fun little way to optimize
			}
			else {
				char const initial = text[0];
				char const number = text[3];
				switch (initial) {
					case 'V': {
						switch (number) {
							case '2': {
								a.setType(EGltfType::VEC2);
								break;
							}
							case '3': {
								a.setType(EGltfType::VEC3);
								break;
							}
							case '4': {
								a.setType(EGltfType::VEC4);
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
								a.setType(EGltfType::MAT2);
								break;
							}
							case '3': {
								a.setType(EGltfType::MAT3);
								break;
							}
							case '4': {
								a.setType(EGltfType::MAT4);
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

		if (!cn.is_null()) {
			a.setCount(static_cast<std::uint32_t>(cn.get_int64().value()));
		}
	}

	static CGltfBuffer parseBuffers(ondemand::value &object) {
		if (auto uri = object["uri"]; uri.has_value()) {
			auto text = uri.value().get_wobbly_string().value();
			char *chars = new char[text.length() + 1];
			std::memset(chars, 0, text.length() + 1);
			text.copy(chars, text.length());
			std::fstream file(chars, std::ios::in | std::ios::binary);
			delete[] chars;
			assert(file.is_open());
		
			std::vector<char> data;
			file.seekg(0, std::ios::end);
			data.resize(static_cast<std::size_t>(file.tellg()));
			std::cout << file.tellg() << '\n';
			file.seekg(0, std::ios::beg);
			file.read(
				data.data(),
				static_cast<std::streamsize>(data.size())
			);

			// validate buffer
			bool any_valid = false;
			for (auto i : data) {
				if (i > 0) {any_valid = true; break;}
				//std::cout << static_cast<i64>(i) << '\n';
			}
			assert(any_valid);
			std::cout << data.size() << '\n';
		
			return {std::move(data)};
		}
		else {
			// TODO; this isnt common practice so just add it later.
		}
		return {};
	}

	GltfMesh_t parseMeshes(ondemand::value &object) {
		GltfMesh_t mesh;

		for (auto prims : object["primitives"]) {
			GltfMeshPrimitive_t primitive;

			for (auto attrib : prims["attributes"].get_object())
				
			}
		}
	}
}

GltfData_t gltf::parse(padded_string &&file) {
	GltfData_t gltf_data;
	ondemand::parser parser;
	const simdjson_result json = std::move(file);

	ondemand::document doc = parser.iterate(json);
	auto obj = doc.get_object();

	for (
		ondemand::value accessors = obj["accessors"].value();
		simdjson_result accessor : accessors
	) {
		assert(accessor.has_value());
		CGltfAccessor a;
		parseAccessors(a, accessor.value());
		std::cout << "buffer view " << a.bufferView() << '\n' << "type " << to_string(a.type()) << '\n' << "comp type " << to_string(a.componentType()) << '\n' << "count " << a.count() << '\n' << '\n';
		gltf_data.accessors.emplace_back(a);
	}

	for (
		ondemand::value buffers = obj["buffers"].value();
		simdjson_result buffer : buffers
	) {
		assert(buffer.has_value());
		CGltfBuffer buf = parseBuffers(buffer.value());
		gltf_data.buffers.emplace_back(std::move(buf));
	}
	
	return gltf_data;
}