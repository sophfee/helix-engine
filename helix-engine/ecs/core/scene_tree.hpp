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

	_NODISCARD Result<RID> create_entity();
	_NODISCARD Error destroy_entity(RID id);
	void set_root(RID const uid);
	_NODISCARD RID get_root() const;
	_NODISCARD Entity* get_entity(RID entity_rid);
	_NODISCARD const Entity *get_entity(const RID entity_rid) const;
	
	_NODISCARD Vector<Entity*> get_entities() const;
	
	void init_frame(f64 delta_time);
	void init_draw(RenderPassInfo const &info);
	void init_render_setup(RenderPassInfo const &info);
	void send_mouse_event(MouseInputEvent const &event);
	void render_extra_passes();
	
	void draw_editors();
	_NODISCARD SharedPtr<Window> get_window() const;

	static void setup_render_pass(RenderPassInfo const &info);
	
	template <typename Fn, typename ...TArgs>
	void visit_component(Fn &&fn, [[maybe_unused]] RID on, TArgs &&...args) {
		using TypeComponent = first_arg<Fn>::type;
		using TypeStripped = std::remove_pointer_t<std::decay_t<TypeComponent>>;
		
		if constexpr (std::is_same_v<TypeStripped, Component>) {
			const SlotPool<IComponentProvider::ProviderComponent> &provider_components = IComponentProvider::provider_components;
			for (const Slot<IComponentProvider::ProviderComponent>& kv : provider_components.slots_) {
				const IComponentProvider::ProviderComponent *provider_component = &kv.value;
				IComponentProvider **provider = IComponentProvider::providers.get(provider_component->provider);
				Component *component = (*provider)->get_component(provider_component->component);
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
	void visit_entity(Fn &&fn, RID on, TArgs &&...args) {
		if (!entities_.is_alive(on))
			on = root_id_;
		Entity* ent = entities_.get(on);
		fn(ent, std::forward<TArgs>(args)...);
		for (RID child : ent->children_)
			visit_entity(std::forward<Fn>(fn), child, std::forward<TArgs>(args)...); // recursive down the scene tree.
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
