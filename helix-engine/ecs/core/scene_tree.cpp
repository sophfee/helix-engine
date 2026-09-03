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

Result<RID> SceneTree::create_entity() {
	Entity entity(shared_from_this(), _STD nullopt, 0);
	SlotPool<Entity>::Handle handle = entities_.emplace(std::move(entity));
	handle->scene_tree_ = shared_from_this();
	handle->unique_id_ = handle;
	handle->parent_id_ = root_id_;
	handle->is_root_ = false;
	
	return {handle};
}

Error SceneTree::destroy_entity(RID id) {
	// Get entity there
	
	Entity* entity = entities_.get(id);
	
	if (entity->is_destroyed_)
		return OK; // It's ok :]
	
	entity->is_enabled_ = false;
	entity->is_destroyed_ = true;
	
	Error err = OK;
	for (RID const cid : entity->children_) {
		err = destroy_entity(cid); // Recursive freeing.
		assert(err == OK);
	}

	entity->children_.clear();
	entity->children_.shrink_to_fit();

	for (const GLID rid : entity->components_) {
		IComponentProvider::ProviderComponent* pc = IComponentProvider::provider_components.get(rid.global);//.provider->removeFrom(entity->id());
		IComponentProvider** p = IComponentProvider::providers.get(pc->provider);
		(*p)->remove_from(entity->get_id());
	}

	entity->components_.clear();
	entity->components_.shrink_to_fit();

	// Remove myself from my parent

	entity->name_ = "deleted ent";
	
	assert(entities_.erase(id));
	
	return OK;
}
void SceneTree::set_root(RID const root_rid) {
	if (root_id_ != RID{0, 0})
		entities_.get(root_id_)->is_root_ = false;
	entities_.get(root_rid)->is_root_ = true;
	root_id_ = root_rid;
}

Entity* SceneTree::get_entity(const RID entity_rid) {
	return entities_.get(entity_rid);
}

const Entity *SceneTree::get_entity(const RID entity_rid) const {
	return entities_.get(entity_rid);
}

Vector<Entity*> SceneTree::get_entities() const {
	Vector<Entity*> result;
	for (auto ent : entities_)
		result.push_back(std::addressof(const_cast<Entity &>(ent.second.get())));
	return result;
}

void SceneTree::init_frame(f64 delta_time) {
	delta_time_ = delta_time;
	visit_component([](Component *component, f64 const delta) {
		component->update(delta);
	}, root_id_, delta_time);
}
void SceneTree::init_draw(RenderPassInfo const &info) {
	setup_render_pass(info);
	visit_component([](Component *component, RenderPassInfo const &p_info) {
		component->draw(p_info);
	}, root_id_, info);
}

void SceneTree::init_render_setup(RenderPassInfo const &info) {
	setup_render_pass(info);
	gpu_check;
	
	visit_component([](Component *component, RenderPassInfo const &p_info) {
		component->render_setup(p_info);
	}, root_id_, info);
}

void SceneTree::send_mouse_event(MouseInputEvent const &event) {
	visit_component([](Component *component, MouseInputEvent const &ev) {
		component->mouse(ev);
	}, root_id_, event);
}

void SceneTree::render_extra_passes() {
	visit_component([this](Component *component) {
		Optional<RenderPassInfo> const pass_info = component->get_custom_render_pass();
		if (pass_info.has_value()) {
			setup_render_pass(pass_info.value());
			init_draw(pass_info.value());
			gpu_check;
		}
	}, root_id_);
	ivec4 const vp = get_window()->viewport();
	assert(vp.z > 0 && vp.w > 0);
}

void SceneTree::draw_editors() {
	entities_.get(root_id_)->editor();
}

SharedPtr<Window> SceneTree::get_window() const {
	return window_;
}

void SceneTree::setup_render_pass(RenderPassInfo const &info) {
}

void SceneTree::dispose() {
	for (const std::pair entity : entities_){
		printf("Disposing entity %s\n", entity.second->name_.c_str());
		if (!entity.second->is_destroyed_) {
			printf("Entity %s is not destroyed, forcing destroy\n", entity.second->name_.c_str());
			assert(destroy_entity(entity.first) == OK);
		}
}
	this->entities_.clear();
	this->window_ = nullptr; // dec ref
}
bool SceneTree::disposed() const {
	return !window_ || window_->disposed();
}