#include "scene_tree.hpp"

#include <cassert>

#include "component.hpp"
#include "engine/filesystem.hpp"
#include "gpu/graphics.hpp"
#include "gpu/window.hpp"

//
// SceneTree
//

SceneTree::SceneTree(SharedPtr<Window> const &window)
	: window_(window) {
}

SceneTree::~SceneTree() {
	//< Nothing as of now.
	dispose();
}

Result<RID> SceneTree::createEntity() {
	Entity entity(shared_from_this(), _STD nullopt, 0);
	SlotPool<Entity>::Handle handle = entities_.emplace(std::move(entity));
	handle->scene_tree_ = shared_from_this();
	handle->unique_id_ = handle;
	handle->parent_id_ = 0;
	return {handle};
}

Error SceneTree::removeEntity(RID id) {
	// Get entity there
	
	Entity* entity = entities_.get(id);
	
	if (!entity->is_destroyed_)
		return OK; // It's ok :]
	
	entity->is_enabled_ = false;
	entity->is_destroyed_ = true;
	
	Error err = OK;
	for (RID const cid : entity->children_) {
		err = removeEntity(cid); // Recursive freeing.
		assert(err == OK);
	}

	entity->children_.clear();
	entity->children_.shrink_to_fit();

	for (const GLID rid : entity->components_) {
		IComponentProvider::ProviderComponent* pc = IComponentProvider::provider_components.get(rid.global);//.provider->removeFrom(entity->id());
		IComponentProvider** p = IComponentProvider::providers.get(pc->provider);
		(*p)->removeFrom(entity->id());
	}

	entity->components_.clear();
	entity->components_.shrink_to_fit();

	// Remove myself from my parent

	entity->name_ = "deleted ent";
	
	assert(entities_.erase(id));
	
	return OK;
}
void SceneTree::setRoot(RID const root_rid) {
	if (root_id_ != RID{0, 0})
		entities_.get(root_id_)->is_root_ = false;
	entities_.get(root_rid)->is_root_ = true;
	root_id_ = root_rid;
}

Entity* SceneTree::entityMut(const RID entity_rid) {
	return entities_.get(entity_rid);
}

const Entity *SceneTree::entity(const RID entity_rid) const {
	return entities_.get(entity_rid);
}

Vector<Entity*> SceneTree::entities() const {
	Vector<Entity*> result;
	for (auto ent : entities_)
		result.push_back(std::addressof(const_cast<Entity &>(ent.second.get())));
	return result;
}

void SceneTree::initiateFrame(f64 delta_time) {
	delta_time_ = delta_time;
	visitComponent([](Component *component, f64 const delta) {
		component->update(delta);
	}, root_id_, delta_time);
}
void SceneTree::initiateDraw(RenderPassInfo const &info) {
	setupRenderPass(info);
	visitComponent([](Component *component, RenderPassInfo const &p_info) {
		component->draw(p_info);
	}, root_id_, info);
}

void SceneTree::initiateRenderSetup(RenderPassInfo const &info) {
	setupRenderPass(info);
	gpu_check;
	
	visitComponent([](Component *component, RenderPassInfo const &p_info) {
		component->renderSetup(p_info);
	}, root_id_, info);
}

void SceneTree::sendMouseEvent(MouseInputEvent const &event) {
	visitComponent([](Component *component, MouseInputEvent const &ev) {
		component->mouse(ev);
	}, root_id_, event);
}

void SceneTree::renderExtraPasses() {
	visitComponent([this](Component *component) {
		Optional<RenderPassInfo> const pass_info = component->customRenderPass();
		if (pass_info.has_value()) {
			setupRenderPass(pass_info.value());
			initiateDraw(pass_info.value());
			gpu_check;
		}
	}, root_id_);
	ivec4 const vp = window()->viewport();
	assert(vp.z > 0 && vp.w > 0);
}

void SceneTree::drawEditors() {
	entities_.get(root_id_)->editor();
}

SharedPtr<Window> SceneTree::window() const {
	return window_;
}

void SceneTree::setupRenderPass(RenderPassInfo const &info) {
}

void SceneTree::dispose() {
	for (const std::pair entity : entities_)
		if (!entity.second->is_destroyed_)
			assert(removeEntity(entity.first) == OK);

	this->entities_.clear();
	this->window_ = nullptr; // dec ref
}
bool SceneTree::disposed() const {
	return !window_ || window_->disposed();
}