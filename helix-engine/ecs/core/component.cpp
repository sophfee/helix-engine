#include "component.hpp"

#include "gpu/graphics.hpp"


Component::Component(Weak<SceneTree> const &scene_tree, Weak<Entity> const &entity):
	tree(scene_tree), entity(entity) {
}
Component::~Component() = default;

void Component::init() {}
void Component::destroy() {
	SharedPtr<Entity> ent = entity.lock();
	ComponentProvider<_STD remove_cvref_t<decltype(*this)>>::remove(ent);
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

SharedPtr<Window> Component::window() const {
	return entity.lock()->window();
}
SharedPtr<SceneTree> Component::sceneTree() const {
	return tree.lock();
}

ivec4 Component::viewport() const {
	return window()->viewport();
}

ComponentProvider<Component> ComponentProvider<Component>::instance_ = ComponentProvider();
