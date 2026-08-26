#pragma once
#include "ecs/3d/camera.hpp"

class EditorCamera3D : public Camera3D {
public:
	EditorCamera3D();
	EditorCamera3D(SharedPtr<SceneTree> const &scene_tree, const RID ent);

	void update(double) override;
	void mouse(MouseInputEvent const &event) override;
	void destroy() override;

private:
	vec2 yawPitch{0.0f, 0.0f};
	bool captured_ = false;
};