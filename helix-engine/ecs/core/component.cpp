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
Optional<RenderPassInfo> Component::customRenderPass() const { return std::nullopt; }
void Component::renderSetup(RenderPassInfo const &info) {}
void Component::draw(RenderPassInfo const &info) {}
void Component::mouse(MouseInputEvent const &event) {}
void Component::editor() {}

Entity * Component::entity() {
	return const_cast<Entity *>(sceneTree()->entity(entity_id));
}

const Entity * Component::entity() const {
	return sceneTree()->entity(entity_id);
}

SharedPtr<Window> Component::window() const {
	return entity()->window();
}

SharedPtr<const SceneTree> Component::sceneTree() const {
	return tree.lock();
}

ivec4 Component::viewport() const {
	return window()->viewport();
}

ComponentProvider<Component> ComponentProvider<Component>::instance_ = ComponentProvider();

