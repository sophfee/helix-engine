// ReSharper disable CppTooWideScopeInitStatement
#pragma once
#include <functional>
#include <iostream>
#include "core_includes.hpp"
#include "entity.hpp"
#include "component.hpp"
#include "engine/disposable.hpp"
#include "engine/main-loop.hpp"

struct RenderPassInfo;
class Window;


template <typename Fn>
struct ComponentVisitorInvoker {
	using component_type = std::remove_pointer_t<typename first_arg<Fn>::type>;

	template <typename... Args>
	static void invoke(Fn &fn, Component *c, Args &&...args) {
		if (component_type *casted = dynamic_cast<component_type *>(c))
			fn(casted, std::forward<Args>(args)...);
	}
};

class SceneTree final : public IDisposable,  public _STD enable_shared_from_this<SceneTree> {
public:
	SceneTree(SharedPtr<Window> const &window);
	~SceneTree() override;

	SceneTree(SceneTree&&) = delete;
	SceneTree(SceneTree const &) = delete;
	SceneTree& operator=(SceneTree&&) = delete;
	SceneTree& operator=(SceneTree const &) = delete;

	_NODISCARD Result<RID> createEntity();
	_NODISCARD Error removeEntity(RID id);
	void setRoot(RID const uid);
	_NODISCARD Entity* entityMut(RID entity_rid);
	_NODISCARD const Entity *entity(const RID entity_rid) const;
	
	_NODISCARD Vector<Entity*> entities() const;
	
	void initiateFrame(f64 delta_time);
	void initiateDraw(RenderPassInfo const &info);
	void initiateRenderSetup(RenderPassInfo const &info);
	void sendMouseEvent(MouseInputEvent const &event);
	void renderExtraPasses();
	
	void drawEditors();
	_NODISCARD SharedPtr<Window> window() const;

	static void setupRenderPass(RenderPassInfo const &info);
	
	template <typename Fn, typename ...TArgs>
	void visitComponent(Fn &&fn, [[maybe_unused]] RID on, TArgs &&...args) {
		using TypeComponent = first_arg<Fn>::type;
		using TypeStripped = std::remove_pointer_t<std::decay_t<TypeComponent>>;
		
		if constexpr (std::is_same_v<TypeStripped, Component>) {
			const SlotPool<IComponentProvider::ProviderComponent> &provider_components = IComponentProvider::provider_components;
			for (const Slot<IComponentProvider::ProviderComponent>& kv : provider_components.slots_) {
				const IComponentProvider::ProviderComponent *provider_component = &kv.value;
				IComponentProvider **provider = IComponentProvider::providers.get(provider_component->provider);
				Component *component = (*provider)->getComponent(provider_component->component);
				fn(component, std::forward<TArgs>(args)...);
			}
		}
		else {
			using Provider = ComponentProvider<std::remove_pointer_t<std::decay_t<TypeComponent>>>;
			using EntityInfo = Provider::EntInfo;
			Provider &provider = Provider::instance_;
			for (const Slot<EntityInfo>& kv : provider.components_.slots_) {
				EntityInfo *ent_info = &kv.value;
				fn((TypeComponent)ent_info, std::forward<TArgs>(args)...);
			}
		}
	}

	template <typename Fn, typename ...TArgs>
	void visitEntity(Fn &&fn, RID on, TArgs &&...args) {
		if (!entities_.is_alive(on))
			on = root_id_;
		Entity* ent = entities_.get(on);
		fn(ent, std::forward<TArgs>(args)...);
		for (RID child : ent->children_)
			visitEntity(std::forward<Fn>(fn), child, std::forward<TArgs>(args)...); // recursive down the scene tree.
	}
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

private:
	SlotPool<Entity> entities_;
	SharedPtr<Window> window_;
	
	//< So to keep the entity list contiguous and without needing to
	//  reallocate lots and update shit like crazy, we put freed
	//  entities in here to be swapped out with new entities.
	RID root_id_ = 0u;
	f64 delta_time_ = 0.0;
	f64 last_frame_time_ = 0.0;
};
