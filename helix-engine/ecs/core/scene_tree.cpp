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

Result<uid> SceneTree::createEntity() {
	if (!empty_slots_.empty())
		return createEntityFromVacantAllocatedSlot_();

	// Check allocated space
	if (entities_.size() == entities_.capacity())
		entities_.reserve(entities_.capacity() + 128); // allocate in big chunks because doing this often sucks ass.
	entities_.push_back(_STD make_shared<Entity>( shared_from_this(), _STD nullopt, entities_.size()));
	return entities_.back()->unique_id_;
}

Error SceneTree::removeEntity(uid id) {
	// Get entity there
	
	if (!entities_[id]->is_destroyed_)
		return OK; // It's ok :]
	
	if (id >= entities_.size())
		return ERR_OUT_OF_RANGE;
	
	SharedPtr<Entity> const ent = entities_.at(id);
	ent->is_enabled_ = false;
	ent->is_destroyed_ = true;
	
	Error err = OK;
	for (uid const cid : ent->children_) {
		
		err = removeEntity(cid); // Recursive freeing.
		assert(err == OK);
	}

	ent->children_.clear();
	ent->children_.shrink_to_fit();

	for (Component *c : ent->components_) {
		c->destroy();
	}

	ent->components_.clear();
	ent->components_.shrink_to_fit();

	// Remove myself from my parent

	ent->name_ = "deleted ent";
	
	empty_slots_.push(ent->id());
	return OK;
}
void SceneTree::setRoot(uid const uid) {
	if (root_id_ != UINT32_MAX)
		entities_[root_id_]->is_root_ = false;
	entities_[uid]->is_root_ = true;
	root_id_ = uid;
}

SharedPtr<Entity> SceneTree::entity(uid const idx) {
	assert(idx < entities_.size());
	return entities_[idx];
}

Vec<SharedPtr<Entity>> const & SceneTree::entities() const {
	assert(!entities_.empty());
	return entities_;
}

void SceneTree::initiateFrame(f64 deltaTime) {
	delta_time_ = deltaTime;
	visitComponent([](Component *component, f64 const dt) {
		component->update(dt);
	}, root_id_, deltaTime);
}
void SceneTree::initiateDraw(RenderPassInfo const &info) {
	setupRenderPass(info);
	visitComponent([](Component *component, RenderPassInfo const &p_info) {
		component->draw(p_info);
		
		gpu_check;
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

void SceneTree::drawEditors() const {
	entities_[root_id_]->editor();
}

SharedPtr<Window> SceneTree::window() const {
	return window_;
}

void SceneTree::setupRenderPass(RenderPassInfo const &info) {
}

Result<uid> SceneTree::createEntityFromVacantAllocatedSlot_() {
	uid const ent_id = empty_slots_.front();
	empty_slots_.pop();
	SharedPtr<Entity> const ent = entities_.at(ent_id);
	ent->name_ = "name";
	ent->unique_id_ = ent_id;
	return ent_id;
}

void SceneTree::dispose() {
	for (const SharedPtr<Entity> &entity : entities_)
		if (!entity->is_destroyed_)
			assert(removeEntity(entity->unique_id_) == OK);

	this->entities_.clear();
	this->window_ = nullptr; // dec ref
}
bool SceneTree::disposed() const {
	return !window_ || window_->disposed();
}