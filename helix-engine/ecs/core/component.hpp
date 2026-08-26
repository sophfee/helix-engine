#pragma once
#include <cassert>

#include "ecs/core/core_includes.hpp"
#include "entity.hpp"
#include "math.hpp"

class Window;
class SceneTree;
struct RenderPassInfo;

class Component {
public:
	Component();
	Component(Weak<SceneTree> const &scene_tree, RID entity);
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
	RID entity_id;
	
	_NODISCARD Entity* entity();
	_NODISCARD const Entity* entity() const;
	
	_NODISCARD SharedPtr<Window> window() const;
	_NODISCARD SharedPtr<const SceneTree> sceneTree() const;
	_NODISCARD ::ivec4 viewport() const;
};

class IComponentProvider : public IDisposable {
public:
	inline static SlotPool<IComponentProvider*> providers = {};
	struct ProviderComponent {
		RID provider;
		RID component;
	};
	inline static SlotPool<ProviderComponent> provider_components = {};
	
	virtual void removeFrom(RID entity) = 0;
	virtual Component* getComponent(RID local_id) = 0;
	
	static void dispose_all() {
		for (const std::pair provider : providers)
			(*provider.second)->dispose();
	}
};

template <typename T>
class ComponentProvider final : public IComponentProvider, EntityFriend {
	inline static const char *type_name = typeid(T).raw_name();
	
	static_assert(std::is_pointer_v<T> == false, "ComponentProvider cannot be used with pointer types.");
	static_assert(std::is_reference_v<T> == false, "ComponentProvider cannot be used with reference types.");
	static_assert(std::is_base_of_v<Component, T>, "Component class must be derived from Component");
	
	using TComp = std::decay_t<T>;
public:
	struct EntInfo {
		T component;
		RID entity_id;
	};
	static ComponentProvider instance_;
	
	RID provider_id;
	
	SlotPool<EntInfo> components_;// = std::make_unique<Vector<TComp>>();
	
	ComponentProvider() {
		provider_id = providers.emplace(this);
	}
	~ComponentProvider() override = default;

	ComponentProvider(ComponentProvider const &) = delete;
	ComponentProvider& operator=(ComponentProvider const &) = delete;
	ComponentProvider(ComponentProvider &&) = delete;
	ComponentProvider& operator=(ComponentProvider &&) = delete;

	_NODISCARD static GLID create(const Entity* entity);
	static void remove(const Entity*  entity);

	void removeFrom(RID entity_rid) override {
		if (!components_.contains(entity_rid)) return;
		for (const std::pair<RID, EntInfo*> ent_info : components_)
			if (ent_info.second->entity_id == entity_rid) {
				components_.erase(ent_info.first);
				break;
			}
#ifdef _DEBUG
			printf("[ComponentProvider<%s>]: Freeing component from %d\n", type_name, entity_rid);
#endif
	}

	static void remove(RID entity_rid);
	_NODISCARD static TComp &get(const Entity* entity);
	_NODISCARD static TComp *get_pointer(const Entity* entity);
	_NODISCARD Component* getComponent(RID local_id) override;
	_NODISCARD static bool contains(RID entity);
	_NODISCARD static bool contains(const Entity* entity);
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
};

template <typename T>
GLID ComponentProvider<T>::create(const Entity* entity) {
	assert(!instance_.contains(entity->id()));
	const RID local_component = instance_.components_.emplace(
		T(entity->tree(), entity->id()),
		entity->id()
	);
	
	instance_.components_.get(local_component)->component.entity_id = entity->id();
	
	const RID global_component = provider_components.emplace(ProviderComponent{instance_.provider_id, local_component});
	return {global_component, local_component};
}

template <typename T> void ComponentProvider<T>::remove(const Entity* entity) {
	remove(entity->id());
}

template <typename T> void ComponentProvider<T>::remove(const RID entity_rid) {
	for (std::pair entity_id : instance_.components_)
		if (entity_id.second->entity_id == entity_rid) {
			instance_.components_.erase(entity_id.first);
			break;
		}
}

template <typename T> typename ComponentProvider<T>::TComp & ComponentProvider<T>::get(const Entity* entity) {
	return *get_pointer(entity);
}
template <typename T> typename ComponentProvider<T>::TComp * ComponentProvider<T>::get_pointer(const Entity* entity) {
	std::size_t local_components = entity->components_.size();
	ComponentProvider& instance = instance_;
	std::size_t allocated_components = instance.components_.size();
	
	if (local_components <= allocated_components) {
		const RID target_provider = instance.provider_id;
		for (const GLID glob : entity->components_) {
			ProviderComponent* provider_component = provider_components.get(glob.global);
			if (provider_component->provider == target_provider)
				return (T*)instance.components_.get(provider_component->component);
		}
	}
	else {
		for (const std::pair<RID, EntInfo*>& ent_info : instance.components_) {
			if (ent_info.second->entity_id == entity->id())
				return (T*)ent_info.second;
		}
	}
}

template <typename T> bool ComponentProvider<T>::contains(RID entity) {
	return instance_.components_.contains(entity);
}

template <typename T>
bool ComponentProvider<T>::contains(const Entity *entity) {
	return contains(entity->id());
}

template <typename Ty> Ty &Entity::component() {
	static_assert(_STD is_base_of_v<Component, Ty>, "Component class must be derived from Component");
	using T = _STD remove_cvref_t<Ty>;
	
	std::size_t local_components = this->components_.size();
	ComponentProvider<T>& instance = ComponentProvider<T>::instance_;
	std::size_t allocated_components = instance.components_.size();
	
	if (local_components <= allocated_components) {
		const RID target_provider = instance.provider_id;
		for (const GLID glob : components_) {
			IComponentProvider::ProviderComponent* provider_component = IComponentProvider::provider_components.get(glob.global);
			if (provider_component->provider == target_provider)
				return (T&)*instance.components_.get(provider_component->component);
		}
	}
	else {
		for (const std::pair<RID, typename ComponentProvider<T>::EntInfo*>& ent_info : instance.components_) {
			if (ent_info.second->entity_id == this->id())
				return (T&)*ent_info.second;
		}
	}
	
	const GLID glid = ComponentProvider<T>::create(this);
	components_.push_back(glid);
	return instance.components_.get(glid.local)->component;
}

template <typename T>
const T & Entity::component() const {
	static_assert(_STD is_base_of_v<Component, T>, "Component class must be derived from Component");
	//assert(ComponentProvider<T>::contains(this) && "Component is not yet on this Entity and cannot be constructed in a Const-Qualified context");
	return ComponentProvider<T>::get(this);
}

template <typename Ty> bool Entity::hasComponent() const {
	using T = _STD remove_cvref_t<Ty>;
	return ComponentProvider<T>::contains(this);
}

template <typename T>
void ComponentProvider<T>::dispose() {
	components_.clear();
}

template <typename T>
bool ComponentProvider<T>::disposed() const {
	return components_.empty();
}
template <typename T>
Component* ComponentProvider<T>::getComponent(RID local_id) {
	if (!components_.contains(local_id)) return nullptr;
	return &components_.get(local_id)->component;
}