#pragma once

#ifdef COMPILE_ECS 
#include <assert.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "types.hpp"

class CSceneTree;
class IComponentInstance;

using uid = u32;

class CEntity final {
	std::shared_ptr<CSceneTree> scene_tree_;
	std::string name_;
	
	std::shared_ptr<CEntity> parent_;
	uid unique_id_ = 0u;
	u32 num_children_ = 0u; // Children are not stored by an array of direct references but instead unique IDs and can be retrieved from the SceneTree

	bool is_root_ = false; // is root node, can only have one! if 2 are found an exception will be thrown from SceneTree.
	bool is_created_ = false; // Used so we can have essentially nullable entities.
	bool is_enabled_ = false;
	bool has_awoken_ = false;
	
protected:
	CEntity(
		std::shared_ptr<CSceneTree> p_sceneTree,
		std::optional<std::string> p_name = std::nullopt,
		std::optional<uid> p_uniqueId = std::nullopt
	);

public:
	CEntity();
	~CEntity();

	std::shared_ptr<IComponentInstance> getComponentInstance(uid id) const;
	template <typename T> [[nodiscard]] T getComponent() const;
	
	CEntity(const CEntity&) = delete;
	CEntity& operator=(const CEntity&) = delete;
	CEntity(CEntity&&) = delete;
	CEntity& operator=(CEntity&&) = delete;
	
	friend class CSceneTree;
};

// makes compile time polymorph a valid runtime polymorph
class IComponentInstance {
public:
	virtual ~IComponentInstance() = default;

	virtual void init() = 0;
	virtual void destroy() = 0;
	
	virtual void wake() = 0;
	virtual void sleep() = 0;
	virtual void update(f64 p_deltaTime = 0.0) = 0;
};

template <typename TComponent>
concept CComponent = requires(TComponent component)
{
	(component.init());
	(component.destroy());
	
	(component.entity) -> std::template shared_ptr<CEntity>;

	(component.wake());
	(component.sleep());
	
	(component.update(0.0));
};

template <CComponent TComponent>
class CComponentInstance final : public IComponentInstance {
public:
	void init() override { component_.init(); }
	void destroy() override { component_.destroy(); }
	void wake() override { component_.wake(); }
	void sleep() override { component_.sleep(); }
	void update(f64 p_deltaTime = 0.0) override { component_.update(p_deltaTime); }

private:
	TComponent component_;
};

class CSceneTree final {
public:
	CSceneTree();
	~CSceneTree();

	CSceneTree(const CSceneTree&) = delete;
	CSceneTree& operator=(const CSceneTree&) = delete;
	CSceneTree(CSceneTree&&) = delete;
	CSceneTree& operator=(CSceneTree&&) = delete;

	[[nodiscard]] Error addEntity(std::shared_ptr<CEntity> p_entity);
	[[nodiscard]] Error removeEntity(uid p_uid);
	[[nodiscard]] std::shared_ptr<CEntity> getEntity(uid);
	[[nodiscard]] std::vector<std::shared_ptr<CEntity>> getEntities() const;

	[[nodiscard]] std::shared_ptr<IComponentInstance> getComponent(uid p_uid);

	void initiateFrame();

private:

	
	
	std::unordered_map< // UID -> PTR
		uid,
		std::shared_ptr<CEntity>
	> entities_;
	
	std::unordered_map<
		uid,
		std::unordered_map<
			uid,
			std::shared_ptr<IComponentInstance>
		>
	> components_;

	uid root_id_ = 0u;
};

template <typename T> T CEntity::getComponent() const {
	
}

#endif