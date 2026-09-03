#include "component.hpp"

#include "scene_tree.hpp"
#include "gpu/graphics.hpp"
#include "gpu/window.hpp"


Component::Component() : tree({}), entity_id({UINT32_MAX, UINT32_MAX}) {
}

Component::Component(Weak<SceneTree> const &scene_tree, const RID entity):
	tree(scene_tree), entity_id(entity) {
}
Component::~Component() {
	destroy();
}

void Component::init() {}
void Component::destroy() {
	//SharedPtr<Entity> ent = entity.lock();
	ComponentProvider<std::decay_t<decltype(*this)>>::remove(entity_id);
}
void Component::wake() {}
void Component::sleep() {}
void Component::update(double) {
	//_STD cout << "Component::update" << '\n';
}
Optional<RenderPassInfo> Component::get_custom_render_pass() const { return std::nullopt; }
void Component::render_setup(RenderPassInfo const &info) {}
void Component::draw(RenderPassInfo const &info) {}
void Component::mouse(MouseInputEvent const &event) {}
void Component::editor() {}

Entity * Component::get_entity() {
	return const_cast<Entity *>(get_scene_tree()->get_entity(entity_id));
}

const Entity * Component::get_entity() const {
	return get_scene_tree()->get_entity(entity_id);
}

SharedPtr<Window> Component::get_window() const {
	return get_entity()->get_window();
}

SharedPtr<const SceneTree> Component::get_scene_tree() const {
	return tree.lock();
}

ivec4 Component::get_viewport() const {
	return get_window()->viewport();
}

ComponentProvider<Component> ComponentProvider<Component>::instance_ = ComponentProvider();

