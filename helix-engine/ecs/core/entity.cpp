#include "scene_tree.hpp"
#include "entity.hpp"

#include <cassert>
#include <format>

#include "component.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

// Entity

Entity::Entity(SharedPtr<SceneTree> const &scene_tree, Optional<String> const &name, Optional<RID> const rid) :
	scene_tree_(scene_tree),
	name_(name.value_or("?")),
	unique_id_(rid.value_or({UINT32_MAX, UINT32_MAX})),
	children_(0),
	components_(0) {
}

Entity::Entity() : name_("?") {
}

Entity::~Entity() {
	if (scene_tree_ == nullptr) return;

	Error const err = scene_tree_->destroy_entity(this->unique_id_);
	
	assert(err == OK);
}

Entity* Entity::get_parent() const {
	assert(!is_root_); //< Root has no parent.
	assert(scene_tree_ != nullptr);
	if (parent_id_ == RID{0,0}) return nullptr;
	Entity* parent_entity = scene_tree_->get_entity(parent_id_);
	assert(parent_entity != nullptr);
	return parent_entity;
}

Entity* Entity::get_child(_STD size_t const idx) const {
	assert(scene_tree_ != nullptr);
	assert(idx < children_.size());
	RID const childUid = children_[idx];
	Entity* const child_entity = scene_tree_->get_entity(childUid);
	assert(child_entity != nullptr);
	return child_entity;
}
Vector<Entity*> Entity::get_children() const {
	Vector<Entity*> result(children_.size());
	SharedPtr<SceneTree> const tree = scene_tree_;
	for (RID const child : children_)
		result.push_back(tree->get_entity(child));
	return result;
}

bool Entity::is_root() const {
	return is_root_;
}

void Entity::set_parent(Entity* entity) {
	assert(scene_tree_ != nullptr);
	entity->add_child(this);
}

void Entity::add_child(Entity* entity) {
	assert(scene_tree_ != nullptr);
	children_.emplace_back(entity->get_id());
	if (entity->parent_id_ != RID{0, 0}) {
		if (scene_tree_->get_entity(entity->parent_id_) != nullptr)
			entity->get_parent()->remove_child(entity);
	}
	entity->parent_id_ = unique_id_;
}

void Entity::remove_child(Entity* entity) {
	assert(scene_tree_ != nullptr);
	Entity* const parent = scene_tree_->get_entity(entity->parent_id_);
	assert(parent == this);
	children_.erase(_STD ranges::find(children_, entity->get_id()));
	entity->parent_id_ = {UINT32_MAX, UINT32_MAX};
}

Vector<Component *> Entity::get_components() const {
	Vector<Component *> result(components_.size());
	for (GLID const component : components_) {
		const IComponentProvider::ProviderComponent *pc = IComponentProvider::provider_components.get(component.global);
		IComponentProvider **p = IComponentProvider::providers.get(pc->provider);
		result.push_back((*p)->get_component(pc->component));
	}
	return result;
}

_STD size_t Entity::get_component_count() const {
	return components_.size();
}

RID Entity::get_id() const {
	return unique_id_;
}

SharedPtr<SceneTree> Entity::get_tree() const {
	assert(scene_tree_ != nullptr);
	return scene_tree_;
}

SharedPtr<Window> Entity::get_window() const {
	return scene_tree_->get_window();
}

#ifdef _DEBUG

void Entity::editor() {
	ImGui::Text("Name: ");
	ImGui::SameLine();
	
	if (ImGui::InputText("##entity_name", &name_, ImGuiInputTextFlags_CallbackCompletion)) {
	}
	
	if (!components_.empty()) {
		
		for (const GLID component : components_) {
			const IComponentProvider::ProviderComponent *pc = IComponentProvider::provider_components.get(component.global);
			IComponentProvider **p = IComponentProvider::providers.get(pc->provider);
			(*p)->get_component(pc->component)->editor();
		}
	}
}

#endif
