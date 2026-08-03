#pragma once
#include <cassert>

#include "ecs/core/core_includes.hpp"
#include "entity.hpp"
#include "math.hpp"
#include "scene_tree.hpp"

class Window;
struct RenderPassInfo;

class Component {
public:
	Component(Weak<SceneTree> const &scene_tree, Weak<Entity> const &entity);
	virtual ~Component();
	
	virtual void init();
	virtual void destroy();
	virtual void wake();
	virtual void sleep();
	virtual void update(double);

	
	virtual Optional<RenderPassInfo> customRenderPass() const;
	virtual void renderSetup(RenderPassInfo const &info);
	virtual void draw(RenderPassInfo const &info);
	virtual void mouse(MouseInputEvent const &event);
	
#ifdef _DEBUG
	// Draw ImGui things
	virtual void editor();
#endif
	
	Weak<SceneTree> tree;
	Weak<Entity> entity;
	uid entity_id;
	_NODISCARD SharedPtr<Window> window() const;
	_NODISCARD SharedPtr<SceneTree> sceneTree() const;
	_NODISCARD ::ivec4 viewport() const;
};

class IComponentProvider : public IDisposable {
public:
	inline static Vec<IComponentProvider*> providers = {};
	
	static void dispose_all() {
		for (IComponentProvider* provider : providers) {
			provider->dispose();
		}
	}
};

template <typename T>
class ComponentProvider final : public IComponentProvider, EntityFriend {
	inline static const char *type_name = typeid(T).raw_name();
	
	using TComp = _STD remove_cvref_t<T>;
	struct EntInfo_t {
		uid component_id;
		uid entity_id;
		uid entity_component_index;
		bool is_deleted = false;
		SharedPtr<SceneTree> scene_tree;
	};
	UnorderedMap<uid, EntInfo_t> uid_to_info_;
	Queue<uid> deleted_components_;
public:
	static ComponentProvider instance_;
	UniquePtr<Vec<TComp>> components_ = std::make_unique<Vec<TComp>>();
	
	ComponentProvider() {
		providers.push_back(this);
	}
	~ComponentProvider() = default;

	ComponentProvider(ComponentProvider const &) = delete;
	ComponentProvider& operator=(ComponentProvider const &) = delete;
	ComponentProvider(ComponentProvider &&) = delete;
	ComponentProvider& operator=(ComponentProvider &&) = delete;

	_NODISCARD static TComp *create(SharedPtr<Entity> const &entity);
	static void remove(SharedPtr<Entity> const & entity);
	static void remove(uid entity_id);
	_NODISCARD static TComp &get(SharedPtr<Entity> const &entity);
	_NODISCARD static TComp *get_pointer(SharedPtr<Entity> const &entity);
	_NODISCARD static bool contains(SharedPtr<Entity const> const &entity);
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
};

template <typename T> typename ComponentProvider<T>::TComp * ComponentProvider<T>::create(SharedPtr<Entity> const &entity) {
	assert(!instance_.uid_to_info_.contains(entity->id()));

	if (instance_.components_->capacity() == instance_.components_->size()) {
		if (!instance_.deleted_components_.empty()) {
			uid component_idx = instance_.deleted_components_.front();
			instance_.deleted_components_.pop();
			instance_.uid_to_info_[entity->id()] = EntInfo_t{
				.component_id = component_idx,
				.entity_id = entity->id(),
				.entity_component_index = static_cast<uid>(entity->componentCount()),
				.scene_tree = entity->tree()
			};
				
			TComp &component = (*instance_.components_)[component_idx];
			component.entity = entity;
			component.tree = entity->tree();
				
			return &instance_.components_->at(component_idx);
		}
		instance_.components_->reserve(instance_.components_->capacity() + 128);

		// OK so all of our entities have desync'd
		for (_STD pair<uid, EntInfo_t> kv_ent_info : instance_.uid_to_info_) {
			SharedPtr<Entity> const ent = kv_ent_info.second.scene_tree->entity(kv_ent_info.second.entity_id);
			ent->components_[kv_ent_info.second.entity_component_index] = &(*instance_.components_)[kv_ent_info.second.component_id];
		}
	}
		
	instance_.components_->emplace_back(entity->tree(), entity);
	instance_.uid_to_info_[entity->id()] = EntInfo_t{
		.component_id = static_cast<u32>(instance_.components_->size() - 1llu),
		.entity_id = entity->id(),
		.entity_component_index = static_cast<uid>(entity->componentCount()),
		.scene_tree = entity->tree()
	};
	TComp &component = instance_.components_->back();
	return _STD addressof(component);
}
template <typename T> void ComponentProvider<T>::remove(SharedPtr<Entity> const &entity) {
	if (!instance_.uid_to_info_.contains(entity->id())) return;
	uid const component_index = instance_.uid_to_info_[entity->id()].component_id;
	instance_.uid_to_info_.erase(entity->id());
	instance_.deleted_components_.push(component_index);
#ifdef _DEBUG
	printf("[ComponentProvider<%s>]: Freeing component from %d\n", type_name, entity->id());
#endif
}
template <typename T> void ComponentProvider<T>::remove(uid const entity_id) {
	if (!instance_.uid_to_info_.contains(entity_id)) return;
	uid const component_index = instance_.uid_to_info_[entity_id].component_id;
	instance_.uid_to_info_.erase(entity_id);
	instance_.deleted_components_.push(component_index);
#ifdef _DEBUG
	printf("[ComponentProvider<%s>]: Freeing component from %d\n", type_name, entity_id);
#endif
}


template <typename T> typename ComponentProvider<T>::TComp & ComponentProvider<T>::get(SharedPtr<Entity> const &entity) {

	return (*instance_.components_)[instance_.uid_to_info_[entity->id()].component_id];
}
template <typename T> typename ComponentProvider<T>::TComp * ComponentProvider<T>::get_pointer(SharedPtr<Entity> const &entity) {
	return &(*instance_.components_)[instance_.uid_to_info_[entity->id()].component_id];
}

template <typename T> bool ComponentProvider<T>::contains(SharedPtr<Entity const> const &entity) {
	return instance_.uid_to_info_.contains(entity->id());
}

template <typename Ty> Ty &Entity::component() {
	static_assert(_STD is_base_of_v<Component, Ty>, "Component class must be derived from Component");
	using T = _STD remove_cvref_t<Ty>;
	SharedPtr<Entity> self = shared_from_this();
	if (!ComponentProvider<T>::contains(self)) {
		T *v = ComponentProvider<T>::create(self);
		components_.push_back(dynamic_cast<Component *>(v));
		return *v;
	}
	return ComponentProvider<T>::get(self);
}

template <typename Ty> bool Entity::hasComponent() const {
	using T = _STD remove_cvref_t<Ty>;
	SharedPtr<Entity const> self = shared_from_this();
	return ComponentProvider<T>::contains(self);
}

template <typename T>
void ComponentProvider<T>::dispose() {
	components_.reset();
	uid_to_info_.clear();
}

template <typename T>
bool ComponentProvider<T>::disposed() const {
	return components_.get() == nullptr;
}