#pragma once

#include "ecs/ecs.hpp"
#include "ecs/core/component.hpp"

class Program;
class Shader;
/**
 * The environment manages nearly everything that's related to effects in a 3D scene and is responsible for handling the rendering of such effects in the pipeline.
 */
class Environment : public Component {
public:
	Environment(SharedPtr<SceneTree> const &scene_tree, SharedPtr<Entity> const &entity);

	void renderSky(u32 quad, vec3 sun_dir, mat4 const &view) const;

	void update(double) override;
	void draw(RenderPassInfo const &info) override;

	void editor() override;

protected:

	struct {
		i32 view,
			inverseView,
			projection,
			inverseProjection,
			lightDirection;
	} uniform_lookup_;
	
	void buildSkyShaderProgram();
	
private:
	SharedPtr<Program> sky_program_;
	SharedPtr<Shader> sky_vertex_;
	SharedPtr<Shader> sky_frag_;
};