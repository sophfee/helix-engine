#pragma once

#include "ecs/ecs.hpp"

class Program;
class Shader;

/**
 * The environment manages nearly everything that's related to effects in a 3D scene and is responsible for handling the rendering of such effects in the pipeline.
 */
class Environment : public Component {
public:
	Environment(Weak<SceneTree> const &scene_tree, Weak<Entity> const &entity);

protected:
	void buildSkyShaderProgram();
	
private:
	Program sky_program_;
};