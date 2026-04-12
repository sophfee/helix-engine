#include "scene_tree.hpp"
#include "entity.hpp"

#include <cassert>
#include <format>

#include "component.hpp"
#include "imgui.h"

// Entity

Entity::Entity(SharedPtr<SceneTree> const &p_sceneTree, Optional<String> const &p_name, Optional<uid> const p_uniqueId) :
	scene_tree_(p_sceneTree),
	name_(p_name.value_or("?")),
	unique_id_(p_uniqueId.value_or(UINT32_MAX)),
	children_(0),
	components_(0) {
}

Entity::Entity() : name_("?") {
}

Entity::~Entity() {
	if (scene_tree_.expired()) return;
	Error const err = scene_tree_.lock()->removeEntity(this->unique_id_);
	assert(err == OK);
}

SharedPtr<Entity> Entity::parent() const {
	assert(!is_root_); //< Root has no parent.
	assert(!scene_tree_.expired());
	return scene_tree_.lock()->entity(parent_id_);
}

SharedPtr<Entity> Entity::child(_STD size_t const idx) const {
	assert(!scene_tree_.expired());
	assert(idx < children_.size());
	uid const childUid = children_[idx];
	assert(childUid != UINT32_MAX);
	return scene_tree_.lock()->entity(childUid);
}
Vec<SharedPtr<Entity>> Entity::children() const {
	Vec<SharedPtr<Entity>> result(children_.size());
	for (uid const child : children_)
		result.push_back(scene_tree_.lock()->entity(child));
	return result;
}

bool Entity::root() const {
	return is_root_;
}

void Entity::setParent(SharedPtr<Entity> const &entity) {
	assert(!scene_tree_.expired());
	entity->addChild(shared_from_this());
}

void Entity::addChild(SharedPtr<Entity> const &entity) {
	assert(!scene_tree_.expired());
	children_.push_back(entity->id());
	if (entity->parent_id_ != UINT32_MAX)
		entity->parent()->removeChild(entity);
	entity->parent_id_ = unique_id_;
}

void Entity::removeChild(SharedPtr<Entity> const &entity) {
	assert(!scene_tree_.expired());
	SharedPtr<Entity> parent = scene_tree_.lock()->entity(entity->parent_id_);
	children_.erase(_STD ranges::find(children_, entity->id()));
	entity->parent_id_ = UINT32_MAX;
}

_STD size_t Entity::componentCount() const {
	return components_.size();
}

uid Entity::id() const {
	return unique_id_;
}

SharedPtr<SceneTree> Entity::tree() const {
	assert(!scene_tree_.expired());
	return scene_tree_.lock();
}

SharedPtr<Window> Entity::window() const {
	return scene_tree_.lock()->window();
}

#ifdef _DEBUG

void Entity::editor() {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull;
	if (children_.empty() && components_.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(.33f, .5f, .9f).Value);
	auto const tree_node_id = std::vformat("{} ({})", std::make_format_args(name_, unique_id_));
	if (ImGui::TreeNodeEx(tree_node_id.c_str(), flags)) {
		ImGui::PopStyleColor(1);
		if (!components_.empty())
			for (Component *component : components_) {
				assert(component != nullptr);
				component->editor();
			}

		if (!children_.empty()) {
			
			for (auto id : children_) {
				SharedPtr<Entity> const child = scene_tree_.lock()->entity(id);
				child->editor();
			}
		}

		ImGui::TreePop();
	}
	else {
		ImGui::PopStyleColor(1);
	}
	debug_hovered_ = ImGui::IsItemHovered();
}

#endif