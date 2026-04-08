#pragma once

#include "core_includes.hpp"

class Window;
class Entity final : public _STD enable_shared_from_this<Entity> {
	Weak<SceneTree> scene_tree_;

	uid parent_id_ = UINT32_MAX;
	uid unique_id_ = UINT32_MAX;

	bool is_root_ = false; // is root node, can only have one! if 2 are found an exception will be thrown from SceneTree.
	bool is_created_ = false; // Used so we can have essentially nullable entities.
	bool is_enabled_ = false;
	bool is_destroyed_ = false; //< Sometime an entity will be left allocated but should be effectively freed with no resources added to it or freed off of it.
	bool has_awoken_ = false;
	
protected:
public:
	Vec<uid> children_; // retrieve children from the Scene Tree.
	String name_ = "Default Entity Name";
	Vec<Component*> components_; //< Lifetime is managed by the entity INDIRECTLY, do not just delete like a fool. They are handled by the component server.
	bool debug_hovered_ = false;
	
	Entity(
		SharedPtr<SceneTree> const &p_sceneTree,
		Optional<String> const &p_name = _STD nullopt,
		Optional<uid> p_uniqueId = _STD nullopt
	);

	Entity();
	~Entity();

	_NODISCARD SharedPtr<Entity> parent() const;
	_NODISCARD SharedPtr<Entity> child(_STD size_t const idx) const;
	_NODISCARD Vec<SharedPtr<Entity>> children() const;
	_NODISCARD bool root() const;

	void setParent(SharedPtr<Entity> const &entity);
	void addChild(SharedPtr<Entity> const &entity);
	void removeChild(SharedPtr<Entity> const &entity);
	
	template <typename T> _NODISCARD T &component();
	template <typename T> _NODISCARD bool hasComponent() const;
	_STD size_t componentCount() const;

	_NODISCARD uid id() const;
	_NODISCARD SharedPtr<SceneTree> tree() const;
	_NODISCARD SharedPtr<Window> window() const;

#ifdef _DEBUG
	void editor();
#endif
	
	Entity(Entity const &) = delete;
	Entity& operator=(Entity const &) = delete;
	//Entity(Entity&&) = delete;
	//Entity& operator=(Entity&&) = delete;
	
	friend class SceneTree;
	friend class _STD vector<Entity>;
	friend class EntityFriend;
};