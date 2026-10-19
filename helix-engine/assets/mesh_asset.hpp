#pragma once

#include "asset.hpp"

struct Vertex;

class MeshAsset;

/**
 * \brief MeshBuilder is a utility class for constructing mesh assets from raw vertex and index data. This also handles optimizing and shadow index generation.
 */
class MeshBuilder {
	
	class PrimitiveBuilder {
	public:
		PrimitiveBuilder() = default;
		PrimitiveBuilder(const PrimitiveBuilder&) = delete;
		PrimitiveBuilder& operator=(const PrimitiveBuilder&) = delete;
		PrimitiveBuilder(PrimitiveBuilder&&) = default;
		PrimitiveBuilder& operator=(PrimitiveBuilder&&) = default;
		
		void add_vertex(const Vertex &vertex);
		void set_vertices(const Vector<Vertex> &vertices);
		void add_index(u32 index);
		void set_indices(const Vector<u32> &indices);
		void optimize();
		
		[[nodiscard]] const Vector<Vertex>& get_vertices() const;
		[[nodiscard]] const Vector<u32>& get_indices() const;
		
	private:
		Vector<Vertex> vertices_;
		Vector<u32> indices_;
	};
	
	PrimitiveBuilder* get_active_primitive();
	
public:
	MeshBuilder() = default;
	MeshBuilder(const MeshBuilder&) = delete;
	MeshBuilder& operator=(const MeshBuilder&) = delete;
	MeshBuilder(MeshBuilder&&) = default;
	MeshBuilder& operator=(MeshBuilder&&) = default;
	
	void add_vertex(const Vertex &vertex);
	void set_vertices(const Vector<Vertex> &vertices);
	void add_index(u32 index);
	void set_indices(const Vector<u32> &indices);
	
	void set_active_primitive(u32 index);
	void next();
	
	void optimize();
	void generate_shadow_indices();
	
private:
	
	Vector<PrimitiveBuilder> primitives_;
	/**
	 * \brief Shadow indices can be aggregated to a single buffer for all primitives, unless they need an alpha map, but that's a problem for future me.
	 */
	Vector<u32> shadow_indices_;
	
	PrimitiveBuilder* active_primitive_ = nullptr;
	
public:
	friend class MeshAsset;
};

class MeshAsset : public Asset {
public:
	using SelfType = MeshAsset;
	
	struct Primitive {
		u32 vertex_count;
		u32 index_count;
		u32 vertex_offset;
		u32 index_offset;
		u32 material_index;
	};
	
	[[nodiscard]] virtual bool build(const MeshBuilder &mesh_builder); //< TODO: Add material asset system
	
	[[nodiscard]] u32 get_vertex_count() const;
	[[nodiscard]] u32 get_index_count() const;
	[[nodiscard]] u32 get_shadow_index_count() const;
	
	[[nodiscard]] StringView get_class_name() const override;
	void set(const std::string_view property, const Any &value) override;
	[[nodiscard]] Any get(const std::string_view property) const override;
	
private:
	RID buffer_;
	Vector<Primitive> primitives_;
};
