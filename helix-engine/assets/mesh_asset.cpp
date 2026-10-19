#include "mesh_asset.hpp"

#include "util.hpp"
#include "gpu/mesh.hpp"

void MeshBuilder::PrimitiveBuilder::add_vertex(const Vertex &vertex) {
	vertices_.emplace_back(vertex);
}

void MeshBuilder::PrimitiveBuilder::set_vertices(const Vector<Vertex> &vertices) {
	vertices_ = vertices;
}

void MeshBuilder::PrimitiveBuilder::add_index(u32 index) {
	indices_.emplace_back(index);
}

void MeshBuilder::PrimitiveBuilder::set_indices(const Vector<u32> &indices) {
	indices_ = indices;
}

void MeshBuilder::PrimitiveBuilder::optimize() {
}

const Vector<Vertex> & MeshBuilder::PrimitiveBuilder::get_vertices() const {
	return vertices_;
}

const Vector<u32> & MeshBuilder::PrimitiveBuilder::get_indices() const {
	return indices_;
}

MeshBuilder::PrimitiveBuilder * MeshBuilder::get_active_primitive() {
	if (active_primitive_ == nullptr) {
		primitives_.emplace_back();
		active_primitive_ = &primitives_.back();
	}
	return active_primitive_;
}


void MeshBuilder::add_vertex(const Vertex &vertex) {
	get_active_primitive()->add_vertex(vertex);
}

void MeshBuilder::set_vertices(const Vector<Vertex> &vertices) {
	get_active_primitive()->set_vertices(vertices);
}

void MeshBuilder::add_index(u32 index) {
	get_active_primitive()->add_index(index);
}

void MeshBuilder::set_indices(const Vector<u32> &indices) {
	get_active_primitive()->set_indices(indices);
}

void MeshBuilder::set_active_primitive(u32 index) {
	if (index >= primitives_.size())
		primitives_.resize(index + 1);
	active_primitive_ = &primitives_[index];
}

void MeshBuilder::next() {
	active_primitive_ = nullptr;
	get_active_primitive(); //< This will emplace new Primitive.
}

void MeshBuilder::optimize() {
	for (auto &primitive : primitives_) {
		primitive.optimize();
	}
}

u32 MeshAsset::get_vertex_count() const {
	u32 sum = 0;
	for (const auto &primitive : primitives_)
		sum += primitive.vertex_count;
	return sum;
}

u32 MeshAsset::get_index_count() const {
	u32 sum = 0;
	for (const auto &primitive : primitives_)
		sum += primitive.index_count;
	return sum;
}

u32 MeshAsset::get_shadow_index_count() const {
	u32 sum = 0;
	return sum;
}

StringView MeshAsset::get_class_name() const {
	return "MeshAsset";
}

void MeshAsset::set(const std::string_view property, const Any &value) {
	switch (hash(property)) {
	default:
		Asset::set(property, value);
		break;
	}
}

Any MeshAsset::get(const std::string_view property) const {
	switch (hash(property)) {
	case hash("vertex_count"):
		return get_vertex_count();
	case hash("index_count"):
		return get_index_count();
	case hash("shadow_index_count"):
		return get_shadow_index_count();
	default:
		return Asset::get(property);
	}
}
