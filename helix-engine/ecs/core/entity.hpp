#pragma once

#include "core_includes.hpp"
#include "engine/disposable.hpp"
// global local id, global = can find w/o type reference, local = faster lookup since no need to lookup provider
struct GLID {
	RID global;
	RID local;
};
class Window;
class Entity final : public _STD enable_shared_from_this<Entity> {
public:
	SharedPtr<SceneTree> scene_tree_;
private:
	RID parent_id_ = UINT32_MAX;
	RID unique_id_ = UINT32_MAX;

	bool is_root_ = false; // is root node, can only have one! if 2 are found an exception will be thrown from SceneTree.
	bool is_created_ = false; // Used so we can have essentially nullable entities.
	bool is_enabled_ = false;
	bool is_destroyed_ = false; //< Sometime an entity will be left allocated but should be effectively freed with no resources added to it or freed off of it.
	bool has_awoken_ = false;
	
protected:
public:
	Vector<RID> children_; // retrieve children from the Scene Tree.
	String name_ = "Default Entity Name";
	
	Vector<GLID> components_; //< Lifetime is managed by the entity INDIRECTLY, do not just delete like a fool. They are handled by the component server.
	bool debug_hovered_ = false;
	
	Entity(
		SharedPtr<SceneTree> const &scene_tree,
		Optional<String> const &name = _STD nullopt,
		Optional<RID> rid = _STD nullopt
	);

	Entity();
	~Entity();

	_NODISCARD Entity* get_parent() const;
	_NODISCARD Entity* get_child(_STD size_t const idx) const;
	_NODISCARD Vector<Entity*> get_children() const;
	_NODISCARD bool is_root() const;

	void set_parent(Entity* entity);
	void add_child(Entity* entity);
	void remove_child(Entity* entity);
	
	template <typename T> _NODISCARD T &get_component();
	template <typename T> _NODISCARD const T &get_component() const;
	template <typename T> _NODISCARD bool has_component() const;
	
	_NODISCARD Vector<Component*> get_components() const;
	
	_STD size_t get_component_count() const;

	_NODISCARD RID get_id() const;
	_NODISCARD SharedPtr<SceneTree> get_tree() const;
	_NODISCARD SharedPtr<Window> get_window() const;

#ifdef _DEBUG
	void editor();
#endif
	
	Entity(Entity const &) = delete;
	Entity& operator=(Entity const &) = delete;
	Entity(Entity&&) = default;
	Entity& operator=(Entity&&) = default;
	//Entity(Entity&&) = delete;
	//Entity& operator=(Entity&&) = delete;
	
	friend class SceneTree;
	friend class _STD vector<Entity>;
	friend class EntityFriend;
};