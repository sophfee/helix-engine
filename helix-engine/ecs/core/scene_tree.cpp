#include "scene_tree.hpp"

#include <cassert>

#include "component.hpp"
#include "gpu/graphics.hpp"

//
// SceneTree
//

SceneTree::SceneTree(SharedPtr<Window> const &window)
	: window_(window) {
}

SceneTree::~SceneTree() {
	//< Nothing as of now.
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
	if (!ent->is_root_) {
		err = removeEntity(ent->parent_id_);
		assert(err == OK);
	}

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
	//_STD cout << ">>SceneTree::initiateFrame()\n";
	delta_time_ = deltaTime;
	visitComponent([](Component *component, f64 const dt) {
		component->update(dt);
	}, root_id_, deltaTime);
}
void SceneTree::initiateDraw(RenderPassInfo const &info) {
	if (info.bind_time.has_value())
		glUniform1d(info.bind_time.value(), glfwGetTime());

	setupRenderPass(info);
	
	visitComponent([](Component *component, RenderPassInfo const &p_info) {
		component->draw(p_info);
	}, root_id_, info);
}

void SceneTree::initiateRenderSetup(RenderPassInfo const &info) {
	if (info.bind_time.has_value())
		glUniform1d(info.bind_time.value(), glfwGetTime());

	setupRenderPass(info);
	
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
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}, root_id_);
	ivec4 const vp = window()->viewport();
	assert(vp.z > 0 && vp.w > 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(vp.x, vp.y, vp.z, vp.w);
}

void SceneTree::drawEditors() const {
	entities_[root_id_]->editor();
}

SharedPtr<Window> SceneTree::window() const {
	return window_;
}

void SceneTree::setupRenderPass(RenderPassInfo const &info) {
	if (info.blend.enabled) {
		glEnable(GL_BLEND);
		if (info.blend.src.has_value() && info.blend.dst.has_value()) {
			glBlendFunc(
				static_cast<GLenum>(info.blend.src.value()),
				static_cast<GLenum>(info.blend.dst.value())
			);
		}
	}
	else {
		glDisable(GL_BLEND);
	}
	
	if (info.depth.depth_test) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(static_cast<GLenum>(info.depth.func));
		glDepthRange(info.depth.range.x, info.depth.range.y);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	if (info.cull) {
		glEnable(GL_CULL_FACE);
		glCullFace(static_cast<GLenum>(info.cull_face));
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	glViewport(info.viewport.x, info.viewport.y,
		info.viewport.z, info.viewport.w);

	if (info.shader_program != nullptr) {
		info.shader_program->use();

		if (info.bind_time.has_value())
			glUniform1d(info.bind_time.value(), glfwGetTime());
		
	}
}

Result<uid> SceneTree::createEntityFromVacantAllocatedSlot_() {
	uid const ent_id = empty_slots_.front();
	empty_slots_.pop();
	SharedPtr<Entity> const ent = entities_.at(ent_id);
	ent->name_ = "name";
	ent->unique_id_ = ent_id;
	return ent_id;
}

Error SceneTree::start() {
	return OK;
}

Error SceneTree::iter() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	f64 const time = glfwGetTime();
	delta_time_ = time - last_frame_time_;
	last_frame_time_ = time;
	
	initiateFrame(delta_time_);
	
	window_->swapBuffers();
	glfwPollEvents();
	
	return OK;
}

Error SceneTree::stop() {
	glfwSetWindowShouldClose(window_->window, GLFW_TRUE);
	return OK;
}
