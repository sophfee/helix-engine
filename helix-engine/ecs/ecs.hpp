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

class CEntity;
class CSceneTree;
class Component;
template <typename T> class CComponentServer;
using uid = u32;

class CFriendToEntity {
};

class CEntity final : public _STD enable_shared_from_this<CEntity> {
	CWeakPtr<CSceneTree> scene_tree_;

	uid parent_id_ = UINT32_MAX;
	uid unique_id_ = UINT32_MAX;

	bool is_root_ = false; // is root node, can only have one! if 2 are found an exception will be thrown from SceneTree.
	bool is_created_ = false; // Used so we can have essentially nullable entities.
	bool is_enabled_ = false;
	bool is_destroyed_ = false; //< Sometime an entity will be left allocated but should be effectively freed with no resources added to it or freed off of it.
	bool has_awoken_ = false;
	
protected:
public:
	CVector<uid> children_; // retrieve children from the Scene Tree.
	_STD string name_ = "Default Entity Name";
	CVector<Component*> components_; //< Lifetime is managed by the entity INDIRECTLY, do not just delete like a fool. They are handled by the component server.
	bool debug_hovered_ = false;
	
	CEntity(
		CSharedPtr<CSceneTree> const &p_sceneTree,
		COptional<_STD string> const &p_name = _STD nullopt,
		COptional<uid> p_uniqueId = _STD nullopt
	);

	CEntity();
	~CEntity();

	_NODISCARD CSharedPtr<CEntity> parent() const;
	_NODISCARD CSharedPtr<CEntity> child(_STD size_t const idx) const;
	_NODISCARD CVector<CSharedPtr<CEntity>> children() const;
	_NODISCARD bool root() const;

	void setParent(CSharedPtr<CEntity> const &entity);
	void addChild(CSharedPtr<CEntity> const &entity);
	void removeChild(CSharedPtr<CEntity> const &entity);
	
	template <typename T> _NODISCARD T &component();
	template <typename T> _NODISCARD bool hasComponent() const;
	_STD size_t componentCount() const;

	_NODISCARD uid id() const;
	_NODISCARD CSharedPtr<CSceneTree> tree() const;

#ifdef _DEBUG
	void editor();
#endif
	
	CEntity(CEntity const &) = delete;
	CEntity& operator=(CEntity const &) = delete;
	//CEntity(CEntity&&) = delete;
	//CEntity& operator=(CEntity&&) = delete;
	
	friend class CSceneTree;
	friend class _STD vector<CEntity>;
	friend class CFriendToEntity;
};

class Component {
public:
	Component(CWeakPtr<CSceneTree> const &scene_tree, CWeakPtr<CEntity> const &entity);
	//Component(CSceneTree *tree, CEntity *ent);
	virtual ~Component();

	virtual void init();
	virtual void destroy();

	virtual void wake();
	virtual void sleep();

	virtual void update(double);
#ifdef _DEBUG
	// Draw ImGui things
	virtual void editor();
#endif

	CWeakPtr<CSceneTree> tree;
	CWeakPtr<CEntity> entity;
};

template <typename T>
class CComponentServer final : CFriendToEntity {
	using TComp = _STD remove_cvref_t<T>;
	struct EntInfo_t {
		uid component_id;
		uid entity_id;
		uid entity_component_index;
		CSharedPtr<CSceneTree> scene_tree;
	};
	CUnorderedMap<uid, EntInfo_t> uid_to_info_;
	CQueue<uid> deleted_components_;
public:
	static CComponentServer instance_;
	CVector<TComp> components_;
	CComponentServer() = default;
	~CComponentServer() = default;

	CComponentServer(CComponentServer const &) = delete;
	CComponentServer& operator=(CComponentServer const &) = delete;
	CComponentServer(CComponentServer &&) = delete;
	CComponentServer& operator=(CComponentServer &&) = delete;

	_NODISCARD static TComp *create(CSharedPtr<CEntity> const &entity);

	static void remove(CSharedPtr<CEntity> const & entity) {
		assert(instance_.uid_to_info_.contains(entity->id()));
		uid const component_index = instance_.uid_to_info_[entity->id()].component_id;
		instance_.uid_to_info_.erase(entity->id());
		instance_.deleted_components_.push(component_index);
	}

	_NODISCARD static TComp &get(CSharedPtr<CEntity> const &entity) {
		return instance_.components_[instance_.uid_to_info_[entity->id()].component_id];
	}

	_NODISCARD static TComp *get_pointer(CSharedPtr<CEntity> const &entity) {
		return &instance_.components_[instance_.uid_to_info_[entity->id()].component_id];
	}

	_NODISCARD static bool contains(CSharedPtr<CEntity const> const &entity) {
		return instance_.uid_to_info_.contains(entity->id());
	}
};



class CSceneTree final : public _STD enable_shared_from_this<CSceneTree> {
public:
	CSceneTree();
	~CSceneTree();

	CSceneTree(CSceneTree const &) = delete;
	CSceneTree& operator=(CSceneTree const &) = delete;
	CSceneTree(CSceneTree&&) = delete;
	CSceneTree& operator=(CSceneTree&&) = delete;

	_NODISCARD CResult<uid> createEntity();
	_NODISCARD Error removeEntity(uid id);
	void setRoot(uid uid);
	_NODISCARD CSharedPtr<CEntity> entity(uid);
	_NODISCARD CVector<CSharedPtr<CEntity>> const& entities() const;
	
	void initiateFrame();
	void drawEditors() const;

protected:

	void frame(uid on);
	CResult<uid> createEntityFromVacantAllocatedSlot_();

private:
	CVector<_STD shared_ptr<CEntity>> entities_;
	//< So to keep the entity list contiguous and without needing to
	//  reallocate lots and update shit like crazy, we put freed
	//  entities in here to be swapped out with new entities.
	CQueue<uid> empty_slots_; 
	uid root_id_ = 0u;
};


template <typename Ty> Ty &CEntity::component() {
	static_assert(_STD is_base_of_v<Component, Ty>, "Component class must be derived from Component");
	using T = _STD remove_cvref_t<Ty>;
	CSharedPtr<CEntity> self = shared_from_this();
	if (!CComponentServer<T>::contains(self)) {
		T *v = CComponentServer<T>::create(self);
		components_.push_back(dynamic_cast<Component *>(v));
		return *v;
	}
	return CComponentServer<T>::get(self);
}

template <typename Ty> bool CEntity::hasComponent() const {
	using T = _STD remove_cvref_t<Ty>;
	CSharedPtr<CEntity const> self = shared_from_this();
	return CComponentServer<T>::contains(self);
}
template <typename T> typename CComponentServer<T>::TComp * CComponentServer<T>::create(CSharedPtr<CEntity> const &entity) {
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
			CSharedPtr<CEntity> const ent = kv_ent_info.second.scene_tree->entity(kv_ent_info.second.entity_id);
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
	extern uid createEntityFromGltf(CSharedPtr<CSceneTree> const &scene_tree, data &data);
}