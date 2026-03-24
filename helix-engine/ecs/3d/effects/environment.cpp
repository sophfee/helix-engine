#include "environment.hpp"

#include "gpu/graphics.hpp"

ComponentServer<Environment> ComponentServer<Environment>::instance_ = ComponentServer();

Environment::Environment(Weak<SceneTree> const &scene_tree, Weak<Entity> const &entity): Component(scene_tree, entity) {}

void Environment::buildSkyShaderProgram() {
	Shader compute(gl::ShaderType::ComputeShader, "shaders\\sky.comp");
	sky_program_.attach(compute);
	sky_program_.link();
}
