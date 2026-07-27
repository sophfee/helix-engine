#include "environment.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "ecs/3d/camera.hpp"
#include "gpu/graphics.hpp"

ComponentProvider<Environment> ComponentProvider<Environment>::instance_ = ComponentProvider();

Environment::Environment(SharedPtr<SceneTree> const &scene_tree, SharedPtr<Entity> const &entity) : Component(scene_tree, entity), uniform_lookup_() {
	buildSkyShaderProgram();
}

void Environment::renderSky(u32 const quad, vec3 sun_dir, mat4 const &view) const {
	
}
void Environment::update(double x) {
}

void Environment::draw(RenderPassInfo const &info) {
	
}
void Environment::editor() {
	ImGui::Text("env exists");
}

void Environment::buildSkyShaderProgram() {
}

void Environment::buildToneMapper() {
	
}
