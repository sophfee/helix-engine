#pragma once
#include "ecs/3d/camera.hpp"

class EditorCamera3D : public Camera3D {
public:
	EditorCamera3D(SharedPtr<SceneTree> const &scene_tree, SharedPtr<Entity> const &ent) : Camera3D(scene_tree, ent) {}

	void update(double) override;
	void mouse(MouseInputEvent const &event) override;
	
private:
	bool captured_ = false;
	vec2 yawPitch;
};