#pragma once

#include <assert.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "mesh.hpp"
#include "types.hpp"

class CEntity;
class CSceneTree;
class Component;

using uid = u32;

/*
template <typename TComponent>
concept Component_c = requires(TComponent component)
{
	//(TComponent::TComponent(CSceneTree *, CEntity 8))
	(component.init());
	(component.destroy());
	
	(component.tree) -> std::template convertible_to<std::template add_pointer_t<CSceneTree>>;
	(component.entity) -> std::template add_pointer_t<CEntity>; //< Weak (raw) pointer, never owns this pointer!

	(component.wake());
	(component.sleep());
	
	(component.update(0.0));
};
*/
class CEntity final {
	CSceneTree *scene_tree_;
	CString name_;

	uid parent_id_ = UINT32_MAX;
	uid unique_id_ = UINT32_MAX;
	CVector<uid> children_; // retrieve children from the Scene Tree.
	CVector<Component*> components_; //< Lifetime is managed by the entity INDIRECTLY, do not just delete like a fool. They are handled by the component server.

	bool is_root_ = false; // is root node, can only have one! if 2 are found an exception will be thrown from SceneTree.
	bool is_created_ = false; // Used so we can have essentially nullable entities.
	bool is_enabled_ = false;
	bool is_destroyed_ = false; //< Sometime an entity will be left allocated but should be effectively freed with no resources added to it or freed off of it.
	bool has_awoken_ = false;
	
protected:
public:
	CEntity(
		CSceneTree *p_sceneTree,
		COptional<CString> const &p_name = std::nullopt,
		COptional<uid> p_uniqueId = std::nullopt
	);

	CEntity();
	~CEntity();

	[[nodiscard]] CEntity const& parent() const;
	[[nodiscard]] CEntity & parent();
	[[nodiscard]] CEntity const& child(std::size_t const idx) const;

	void setParent(CEntity & p_entity);
	void addChild(CEntity & p_entity);
	void removeChild(CEntity & p_entity);
	
	template <typename T> [[nodiscard]] T &component();
	template <typename T> [[nodiscard]] bool hasComponent() const;

	[[nodiscard]] uid id() const;
	[[nodiscard]] CSceneTree *tree() const;
	
	CEntity(CEntity const &) = delete;
	CEntity& operator=(CEntity const &) = delete;
	//CEntity(CEntity&&) = delete;
	//CEntity& operator=(CEntity&&) = delete;
	
	friend class CSceneTree;
	friend class std::vector<CEntity>;
};

class Component {
public:
	Component() = default;
	Component(CSceneTree *tree, CEntity *ent);
	virtual ~Component();

	virtual void init();
	virtual void destroy();

	virtual void wake();
	virtual void sleep();

	virtual void update(double);

	CSceneTree *tree = nullptr;
	CEntity *entity = nullptr;
};

template <typename T>
class CComponentServer final {
	using TComp = std::remove_cvref_t<T>;
	CUnorderedMap<uid, uid> uid_to_idx_;
	CVector<TComp> components_;
	CQueue<uid> deleted_components_;
	static CComponentServer instance_;
public:
	CComponentServer() = default;
	~CComponentServer() = default;

	CComponentServer(CComponentServer const &) = delete;
	CComponentServer& operator=(CComponentServer const &) = delete;
	CComponentServer(CComponentServer &&) = delete;
	CComponentServer& operator=(CComponentServer &&) = delete;

	[[nodiscard]] static TComp &create(CEntity const &entity) {
		assert(!instance_.uid_to_idx_.contains(entity.id()));

		if (instance_.components_.capacity() == instance_.components_.size()) {
			if (!instance_.deleted_components_.empty()) {
				uid component_idx = instance_.deleted_components_.front();
				instance_.deleted_components_.pop();
				instance_.uid_to_idx_[entity.id()] = component_idx;
				return instance_.components_.at(component_idx);
			}
			instance_.components_.reserve(instance_.components_.capacity() + 128);
		}
		
		instance_.components_.emplace_back(entity.tree(), const_cast<CEntity *>(&entity));
		instance_.uid_to_idx_[entity.id()] = static_cast<u32>(instance_.components_.size() - 1llu);
		
		return instance_.components_.back();
	}
	
	static void remove(CEntity const & entity) {
		assert(instance_.uid_to_idx_.contains(entity.id()));
		uid const component_index = instance_.uid_to_idx_[entity.id()];
		instance_.uid_to_idx_.erase(entity.id());
		instance_.deleted_components_.push(component_index);
	}

	[[nodiscard]] static TComp &get(CEntity const &entity) {
		return instance_.components_[instance_.uid_to_idx_[entity.id()]];
	}

	[[nodiscard]] static bool contains(CEntity const &entity) {
		return instance_.uid_to_idx_.contains(entity.id());
	}
};

template <typename Ty> Ty &CEntity::component() {
	using T = std::remove_cvref_t<Ty>;
	if (!CComponentServer<T>::contains(*this)) {
		T &v = CComponentServer<T>::create(*this);
		components_.push_back(dynamic_cast<Component *>(const_cast<T *>(&v)));
		return v;
	}
	return CComponentServer<T>::get(*this);
}
template <typename Ty> bool CEntity::hasComponent() const {
	using T = std::remove_cvref_t<Ty>;
	return CComponentServer<T>::contains(*this);
}

class Transform : public Component {
public:
	Transform(CSceneTree *p_tree, CEntity *p_ent) : Component(p_tree, p_ent) {}

	glm::vec3 translation;
	glm::vec3 scale;
	glm::quat rotation;

	[[nodiscard]] glm::mat4 matrix() const;
};

class CMesh;

class Mesh : public Component {
public:
	Mesh(CSceneTree *p_tree, CEntity *p_ent);

	void update(double) override;
	
	CUniquePtr<CMesh> mesh;
};

class CSceneTree final {
public:
	CSceneTree();
	~CSceneTree();

	CSceneTree(CSceneTree const &) = delete;
	CSceneTree& operator=(CSceneTree const &) = delete;
	CSceneTree(CSceneTree&&) = delete;
	CSceneTree& operator=(CSceneTree&&) = delete;

	[[nodiscard]] CResult<uid> createEntity();
	[[nodiscard]] Error removeEntity(uid p_uid);
	void setRoot(uid uid);
	[[nodiscard]] CEntity & entity(uid);
	[[nodiscard]] CVector<CSharedPtr<CEntity>> const& entities() const;
	
	void initiateFrame();

protected:

	void frame(uid on);
	CResult<uid> createEntityFromVacantAllocatedSlot_();

private:
	CVector<std::shared_ptr<CEntity>> entities_;
	//< So to keep the entity list contiguous and without needing to
	//  reallocate lots and update shit like crazy, we put freed
	//  entities in here to be swapped out with new entities.
	CQueue<uid> empty_slots_; 
	uid root_id_ = 0u;
};