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
	sky_program_->use();
	sun_dir = glm::normalize(sun_dir);
	//glDepthFunc(GL_GREATER);
	Camera3D *camera_3d = Camera3D::currentCameraEntity();
	sky_program_->setUniform(uniform_lookup_.view, camera_3d->viewMatrix());
	sky_program_->setUniform(uniform_lookup_.inverseView, camera_3d->inverseViewMatrix());
	sky_program_->setUniform(uniform_lookup_.projection, camera_3d->projectionMatrix());
	sky_program_->setUniform(uniform_lookup_.inverseProjection, camera_3d->inverseProjectionMatrix());
	
	sky_program_->setUniform(uniform_lookup_.lightDirection, sun_dir);
	glBindVertexArray(quad);
	glDepthFunc(GL_EQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDepthFunc(GL_LESS);
}
void Environment::update(double x) {
	sky_program_->integrityCheck();
}

void Environment::draw(RenderPassInfo const &info) {
	if (info.render_sky)
		renderSky(rd::full_screen_quad, glm::normalize(vec3(-0.2F, -1.0F, 0.4F)), Camera3D::currentCameraEntity()->viewMatrix());
}
void Environment::editor() {
	ImGui::Text("env exists");
}

void Environment::buildSkyShaderProgram() {
	sky_program_ = std::make_shared<Program>();
	sky_vertex_ = std::make_shared<Shader>(gl::ShaderType::VertexShader, "shaders\\deferred_shading.vert");
	sky_frag_ = std::make_shared<Shader>(gl::ShaderType::FragmentShader, "shaders\\sky.frag");
	
	sky_program_->attach(*sky_vertex_);
	sky_program_->attach(*sky_frag_);
	sky_program_->link();

	uniform_lookup_.view = sky_program_->uniformLocation("view");
	uniform_lookup_.inverseView = sky_program_->uniformLocation("inverse_view");
	uniform_lookup_.projection = sky_program_->uniformLocation("projection");
	uniform_lookup_.inverseProjection = sky_program_->uniformLocation("inverse_projection");
	
	uniform_lookup_.lightDirection = sky_program_->uniformLocation("light_direction");
}
