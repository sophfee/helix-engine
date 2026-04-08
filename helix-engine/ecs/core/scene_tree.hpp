#pragma once

#include "core_includes.hpp"
#include "entity.hpp"
#include "main-loop.hpp"

struct RenderPassInfo;
class Window;

class SceneTree final : public IMainLoop,  public _STD enable_shared_from_this<SceneTree> {
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
	
	void initiateFrame(f64 deltaTime);
	void initiateDraw(RenderPassInfo const &info);
	void initiateRenderSetup(RenderPassInfo const &info);
	void sendMouseEvent(MouseInputEvent const &event);
	void renderExtraPasses();
	
	void drawEditors() const;
	_NODISCARD SharedPtr<Window> window() const;

	static void setupRenderPass(RenderPassInfo const &info);

	template <typename Fn, typename ...TArgs>
	void visitComponent(Fn &&fn, uid on, TArgs &&...args) {
		if (on == UINT32_MAX)
			on = root_id_;
		SharedPtr<Entity> const ent = entities_.at(on);
		
		for (Component *c : ent->components_)
			fn(c, std::forward<TArgs>(args)...);
		
		for (uid child : ent->children_)
			visitComponent(std::forward<Fn>(fn), child, std::forward<TArgs>(args)...); // recursive down the scene tree.
	}

	template <typename Fn, typename ...TArgs>
	void visitEntity(Fn &&fn, uid on, TArgs &&...args) {
		if (on == UINT32_MAX)
			on = root_id_;
		SharedPtr<Entity> const ent = entities_.at(on);
		fn(ent, std::forward<TArgs>(args)...);
		for (uid child : ent->children_)
			visitEntity(std::forward<Fn>(fn), child, std::forward<TArgs>(args)...); // recursive down the scene tree.
	}
	
/*
	void visitComponent(std::function<void(Component *)> const &fn, uid const on) {
		SharedPtr<Entity> const ent = entities_.at(on);
		
		for (Component *c : ent->components_)
			fn(c);
		
		for (uid const child : ent->children_)
			visitComponent(fn, child); // recursive down the scene tree.
	}
*/
	
	Result<uid> createEntityFromVacantAllocatedSlot_();

public:
	Error start() override;
	Error iter() override;
	Error stop() override;

private:
	Vec<SharedPtr<Entity>> entities_;
	SharedPtr<Window> window_;
	
	//< So to keep the entity list contiguous and without needing to
	//  reallocate lots and update shit like crazy, we put freed
	//  entities in here to be swapped out with new entities.
	Queue<uid> empty_slots_; 
	uid root_id_ = 0u;
	f64 delta_time_ = 0.0;
	f64 last_frame_time_ = 0.0;
};
