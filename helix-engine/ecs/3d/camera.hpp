#pragma once

#include "ecs/ecs.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera3D : public Component {
public:
	Camera3D(SharedPtr<SceneTree> const &scene_tree, SharedPtr<Entity> const &ent);

	_NODISCARD mat4 viewMatrix() const noexcept;
	_NODISCARD mat4 inverseViewMatrix() const noexcept;
	_NODISCARD mat4 projectionMatrix() const noexcept;
	_NODISCARD mat4 inverseProjectionMatrix() const noexcept;
	_NODISCARD mat4 projectionViewMatrix() const noexcept;
	_NODISCARD mat4 inverseProjectionViewMatrix() const noexcept;

	void setFieldOfVision(f32 fov_radians);
	_NODISCARD f32 fieldOfVision() const;

	inline void setFov(f32 const fov_radians) { setFieldOfVision(fov_radians); }
	inline _NODISCARD f32 fov() const { return fieldOfVision(); }

	void setAspectRatio(f32 const aspect_ratio);
	_NODISCARD f32 aspectRatio() const;

	void setFarPlane(f32 const far_plane);
	_NODISCARD f32 farPlane() const;

	void setNearPlane(f32 const near_plane);
	_NODISCARD f32 nearPlane() const;

	void setSize(f32 const left, f32 const right, f32 const bottom, f32 const top);
	_NODISCARD vec4 size() const;

	void renderSetup(RenderPassInfo const &info) override;
	void editor() override;

	void refreshMatrices();

	_NODISCARD static SharedPtr<Entity> currentCameraEntity();

	void makeCurrent();

protected:
	void updateViewMatrix();
	void updateProjectionMatrix();

private:
	static Camera3D *current_camera_;
	
	union CameraAttributes {
		struct PerspectiveCameraAttributes {
			f32 fov_;
			f32 aspect_ratio_;
		} perspective_;
		struct OrthographicCameraAttributes {
			f32 left_;
			f32 right_;
			f32 bottom_;
			f32 top_;
		} orthographic_;
	} camera_attributes_;
	
	f32 near_z_;
	f32 far_z_;
	
protected:
	mat4 view_;
	mat4 projection_;
	mat4 inverse_view_;
	mat4 inverse_projection_;
private:
	bool is_orthographic_;
	bool is_current_;
};
