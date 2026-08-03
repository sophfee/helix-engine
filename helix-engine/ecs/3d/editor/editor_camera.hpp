#pragma once
#include "ecs/3d/camera.hpp"

class EditorCamera3D : public Camera3D {
public:
	EditorCamera3D(SharedPtr<SceneTree> const &scene_tree, SharedPtr<Entity> const &ent);

	void update(double) override;
	void mouse(MouseInputEvent const &event) override;
	void destroy() override;

private:
	vec2 yawPitch;
	bool captured_ = false;
};