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
	children_.push_back(entity->get_id());
	if (entity->parent_id_.valid())
		if (scene_tree_->get_entity(entity->parent_id_) != nullptr)
			entity->get_parent()->remove_child(entity);
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
	using namespace ImGui;
	
	if (BeginTable("##inspector", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		TableSetupColumn("Property", ImGuiTableColumnFlags_WidthStretch);
		TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
		TableHeadersRow();
		TableNextRow();
		TableNextColumn();
		
		//TableSetBgColor(ImGuiTableBgTarget_CellBg, ImColor(0.1f, 0.1f, 0.1f, 1.0f));
		//TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImColor(0.1f, 0.1f, 0.1f, 0.2f));
		//TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImColor(0.2f, 0.2f, 0.2f, 0.5f));
		
		//PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 2));
		Text("Name");
		TableNextColumn();
		SetNextItemWidth(-FLT_MIN);
		InputText("##name", &name_);
		TableNextRow();
		TableNextColumn();
		
		Text("ID");
		TableNextColumn();
		Text("%s", std::format("{:08X}:{:08X}", unique_id_.upper, unique_id_.lower).c_str());
		TableNextRow();
		TableNextColumn();
	
		if (!components_.empty()) {
		
			for (const GLID component : components_) {
				const IComponentProvider::ProviderComponent *pc = IComponentProvider::provider_components.get(component.global);
				IComponentProvider **p = IComponentProvider::providers.get(pc->provider);
				(*p)->get_component(pc->component)->editor();
			}
		}
		
		//PopStyleVar();
		
		EndTable();
	}
}
#endif

EntityRef::EntityRef(SharedPtr<SceneTree> const &scene_tree, RID const entity_unique_id) : scene_tree_(scene_tree), unique_id_(entity_unique_id) {
}

bool EntityRef::is_valid() const {
	assert(scene_tree_ != nullptr);
	return scene_tree_->get_entity(unique_id_) != nullptr;
}

StringView EntityRef::get_name() const {
	assert(scene_tree_ != nullptr);
	const Entity *entity = get_pointer();
	assert(entity != nullptr);
	return entity->name_;
}

void EntityRef::set_name(StringView name) {
	assert(scene_tree_ != nullptr);
	Entity *entity = get_pointer();
	assert(entity != nullptr);
	entity->name_ = name;
}

void EntityRef::get_parent(EntityRef &out_parent) {
	assert(scene_tree_ != nullptr);
	const Entity *entity = get_pointer();
	assert(entity != nullptr);
	if (entity->parent_id_.valid())
		out_parent = EntityRef(scene_tree_, entity->parent_id_);
	else
		out_parent = EntityRef();
}

void EntityRef::set_parent(EntityRef &parent) {
	assert(scene_tree_ != nullptr);
	Entity *entity = get_pointer();
	assert(entity != nullptr);
	entity->set_parent(parent.get_pointer());
}

void EntityRef::get_child(size_t index, EntityRef &out_child) {
	assert(scene_tree_ != nullptr);
	const Entity *entity = get_pointer();
	assert(entity != nullptr);
	assert(index < entity->children_.size());
	out_child = EntityRef(scene_tree_, entity->children_[index]);
}

void EntityRef::add_child(EntityRef &child) {
	assert(scene_tree_ != nullptr);
	Entity *entity = get_pointer();
	assert(entity != nullptr);
	entity->add_child(child.get_pointer());
}

void EntityRef::remove_child(EntityRef &child) {
	assert(scene_tree_ != nullptr);
	Entity *entity = get_pointer();
	assert(entity != nullptr);
	entity->remove_child(child.get_pointer());
}

const Entity * EntityRef::get_pointer() const {
	if (cached_ptr_ == nullptr) 
		cached_ptr_ = scene_tree_->get_entity(unique_id_);
	return cached_ptr_;
}

Entity * EntityRef::get_pointer() {
	if (cached_ptr_ == nullptr)
		cached_ptr_ = scene_tree_->get_entity(unique_id_);
	return cached_ptr_;
}

