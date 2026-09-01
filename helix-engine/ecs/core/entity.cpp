#include "scene_tree.hpp"
#include "entity.hpp"

#include <cassert>
#include <format>

#include "component.hpp"
#include "imgui.h"

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

	Error const err = scene_tree_->removeEntity(this->unique_id_);
	
	assert(err == OK);
}

Entity* Entity::parent() const {
	assert(!is_root_); //< Root has no parent.
	assert(scene_tree_ != nullptr);
	if (parent_id_ == RID{0,0}) return nullptr;
	Entity* parent_entity = scene_tree_->entityMut(parent_id_);
	assert(parent_entity != nullptr);
	return parent_entity;
}

Entity* Entity::child(_STD size_t const idx) const {
	assert(scene_tree_ != nullptr);
	assert(idx < children_.size());
	RID const childUid = children_[idx];
	Entity* const child_entity = scene_tree_->entityMut(childUid);
	assert(child_entity != nullptr);
	return child_entity;
}
Vector<Entity*> Entity::children() const {
	Vector<Entity*> result(children_.size());
	SharedPtr<SceneTree> const tree = scene_tree_;
	for (RID const child : children_)
		result.push_back(tree->entityMut(child));
	return result;
}

bool Entity::root() const {
	return is_root_;
}

void Entity::setParent(Entity* entity) {
	assert(scene_tree_ != nullptr);
	entity->addChild(this);
}

void Entity::addChild(Entity* entity) {
	assert(scene_tree_ != nullptr);
	children_.emplace_back(entity->id());
	if (entity->parent_id_ != RID{0, 0}) {
		if (scene_tree_->entityMut(entity->parent_id_) != nullptr)
			entity->parent()->removeChild(entity);
	}
	entity->parent_id_ = unique_id_;
}

void Entity::removeChild(Entity* entity) {
	assert(scene_tree_ != nullptr);
	Entity* const parent = scene_tree_->entityMut(entity->parent_id_);
	assert(parent == this);
	children_.erase(_STD ranges::find(children_, entity->id()));
	entity->parent_id_ = {UINT32_MAX, UINT32_MAX};
}

Vector<Component *> Entity::components() const {
	Vector<Component *> result(components_.size());
	for (GLID const component : components_) {
		const IComponentProvider::ProviderComponent *pc = IComponentProvider::provider_components.get(component.global);
		IComponentProvider **p = IComponentProvider::providers.get(pc->provider);
		result.push_back((*p)->getComponent(pc->component));
	}
	return result;
}

_STD size_t Entity::componentCount() const {
	return components_.size();
}

RID Entity::id() const {
	return unique_id_;
}

SharedPtr<SceneTree> Entity::tree() const {
	assert(scene_tree_ != nullptr);
	return scene_tree_;
}

SharedPtr<Window> Entity::window() const {
	return scene_tree_->window();
}

#ifdef _DEBUG

void Entity::editor() {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesFull;
	if (children_.empty() && components_.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(.33f, .5f, .9f).Value);
	auto const tree_node_id = std::vformat("{} ({})", std::make_format_args(name_, unique_id_.upper));
	if (ImGui::TreeNodeEx(tree_node_id.c_str(), flags)) {
		ImGui::PopStyleColor(1);
		if (!components_.empty())
			for (const GLID component : components_) {
				const IComponentProvider::ProviderComponent *pc = IComponentProvider::provider_components.get(component.global);
				IComponentProvider **p = IComponentProvider::providers.get(pc->provider);
				(*p)->getComponent(pc->component)->editor();
			}

		if (!children_.empty())
			for (const auto id : children_) {
				Entity *const child = scene_tree_->entityMut(id);
				child->editor();
			}

		ImGui::TreePop();
	}
	else {
		ImGui::PopStyleColor(1);
	}
	debug_hovered_ = ImGui::IsItemHovered();
}

#endif
