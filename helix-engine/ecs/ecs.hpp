#pragma once

#include <assert.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "gpu/mesh.hpp"
#include "types.hpp"

class Entity;
class SceneTree;
class Component;
template <typename T> class ComponentServer;
using uid = u32;

class EntityFriend {
};

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

class Component {
public:
	Component(Weak<SceneTree> const &scene_tree, Weak<Entity> const &entity);
	virtual ~Component();
	
	virtual void init();
	virtual void destroy();
	virtual void wake();
	virtual void sleep();
	virtual void update(double);
	virtual void renderSetup(RenderPassInfo const &info);
	virtual void draw(RenderPassInfo const &info);
	
#ifdef _DEBUG
	// Draw ImGui things
	virtual void editor();
#endif

	Weak<SceneTree> tree;
	Weak<Entity> entity;
	_NODISCARD SharedPtr<Window> window() const;
};

template <typename T>
class ComponentServer final : EntityFriend {
	using TComp = _STD remove_cvref_t<T>;
	struct EntInfo_t {
		uid component_id;
		uid entity_id;
		uid entity_component_index;
		SharedPtr<SceneTree> scene_tree;
	};
	UnorderedMap<uid, EntInfo_t> uid_to_info_;
	Queue<uid> deleted_components_;
public:
	static ComponentServer instance_;
	Vec<TComp> components_;
	ComponentServer() = default;
	~ComponentServer() = default;

	ComponentServer(ComponentServer const &) = delete;
	ComponentServer& operator=(ComponentServer const &) = delete;
	ComponentServer(ComponentServer &&) = delete;
	ComponentServer& operator=(ComponentServer &&) = delete;

	_NODISCARD static TComp *create(SharedPtr<Entity> const &entity);

	static void remove(SharedPtr<Entity> const & entity) {
		assert(instance_.uid_to_info_.contains(entity->id()));
		uid const component_index = instance_.uid_to_info_[entity->id()].component_id;
		instance_.uid_to_info_.erase(entity->id());
		instance_.deleted_components_.push(component_index);
	}

	_NODISCARD static TComp &get(SharedPtr<Entity> const &entity) {
		return instance_.components_[instance_.uid_to_info_[entity->id()].component_id];
	}

	_NODISCARD static TComp *get_pointer(SharedPtr<Entity> const &entity) {
		return &instance_.components_[instance_.uid_to_info_[entity->id()].component_id];
	}

	_NODISCARD static bool contains(SharedPtr<Entity const> const &entity) {
		return instance_.uid_to_info_.contains(entity->id());
	}
};



class SceneTree final : public _STD enable_shared_from_this<SceneTree> {
public:
	SceneTree(SharedPtr<Window> const &window);
	~SceneTree();

	SceneTree(SceneTree&&) = delete;
	SceneTree(SceneTree const &) = delete;
	SceneTree& operator=(SceneTree&&) = delete;
	SceneTree& operator=(SceneTree const &) = delete;

	_NODISCARD Result<uid> createEntity();
	_NODISCARD Error removeEntity(uid id);
	void setRoot(uid uid);
	_NODISCARD SharedPtr<Entity> entity(uid);
	_NODISCARD Vec<SharedPtr<Entity>> const& entities() const;
	
	void initiateFrame();
	void initiateDraw(RenderPassInfo const &info);
	void initiateRenderSetup(RenderPassInfo const &info);
	
	void drawEditors() const;
	_NODISCARD SharedPtr<Window> window() const;

protected:

	void frame(uid on);
	void renderSetup(uid on, RenderPassInfo const &info);
	void draw(uid on, RenderPassInfo const &info);
	Result<uid> createEntityFromVacantAllocatedSlot_();

private:
	Vec<SharedPtr<Entity>> entities_;
	SharedPtr<Window> window_;
	
	//< So to keep the entity list contiguous and without needing to
	//  reallocate lots and update shit like crazy, we put freed
	//  entities in here to be swapped out with new entities.
	Queue<uid> empty_slots_; 
	uid root_id_ = 0u;
};


template <typename Ty> Ty &Entity::component() {
	static_assert(_STD is_base_of_v<Component, Ty>, "Component class must be derived from Component");
	using T = _STD remove_cvref_t<Ty>;
	SharedPtr<Entity> self = shared_from_this();
	if (!ComponentServer<T>::contains(self)) {
		T *v = ComponentServer<T>::create(self);
		components_.push_back(dynamic_cast<Component *>(v));
		return *v;
	}
	return ComponentServer<T>::get(self);
}

template <typename Ty> bool Entity::hasComponent() const {
	using T = _STD remove_cvref_t<Ty>;
	SharedPtr<Entity const> self = shared_from_this();
	return ComponentServer<T>::contains(self);
}
template <typename T> typename ComponentServer<T>::TComp * ComponentServer<T>::create(SharedPtr<Entity> const &entity) {
	assert(!instance_.uid_to_info_.contains(entity->id()));

	if (instance_.components_.capacity() == instance_.components_.size()) {
		if (!instance_.deleted_components_.empty()) {
			uid component_idx = instance_.deleted_components_.front();
			instance_.deleted_components_.pop();
			instance_.uid_to_info_[entity->id()] = EntInfo_t{
				.component_id = component_idx,
				.entity_id = entity->id(),
				.entity_component_index = static_cast<uid>(entity->componentCount()),
				.scene_tree = entity->tree()
			};
				
			TComp &component = instance_.components_[component_idx];
			component.entity = entity;
			component.tree = entity->tree();
				
			return &instance_.components_.at(component_idx);
		}
			
		instance_.components_.reserve(instance_.components_.capacity() + 128);

		// OK so all of our entities have desync'd
		for (_STD pair<uid, EntInfo_t> kv_ent_info : instance_.uid_to_info_) {
			SharedPtr<Entity> const ent = kv_ent_info.second.scene_tree->entity(kv_ent_info.second.entity_id);
			ent->components_[kv_ent_info.second.entity_component_index] = &instance_.components_[kv_ent_info.second.component_id];
		}
	}
		
	instance_.components_.emplace_back(entity->tree(), entity);
	instance_.uid_to_info_[entity->id()] = EntInfo_t{
		.component_id = static_cast<u32>(instance_.components_.size() - 1llu),
		.entity_id = entity->id(),
		.entity_component_index = static_cast<uid>(entity->componentCount()),
		.scene_tree = entity->tree()
	};
	TComp &component = instance_.components_.back();
	return _STD addressof(component);
}


namespace gltf {
	struct data;
	
	/**
	 * @see gltf::parse
	 * @param scene_tree
	 * @param data The gltf data that has been parsed already
	 * @return The root entity id, note that it has yet to be added to the scene hierarchy as it is not the child of any entity.
	 */
	extern uid createEntityFromGltf(SharedPtr<SceneTree> const &scene_tree, data &data);
}