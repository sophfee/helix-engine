#pragma once

#include "ecs/ecs.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ecs/core/component.hpp"

struct GpuCameraData {
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
	mat4 inverseView;
	mat4 inverseProjection;
	mat4 inverseViewProjection;
};

class Camera3D : public Component {
public:
	Camera3D();
	Camera3D(SharedPtr<SceneTree> const &scene_tree, const RID ent);

	_NODISCARD mat4 get_view() const noexcept;
	_NODISCARD mat4 get_inverse_view() const noexcept;
	_NODISCARD mat4 get_projection() const noexcept;
	_NODISCARD mat4 get_inverse_projection() const noexcept;
	_NODISCARD mat4 get_projection_view() const noexcept;
	_NODISCARD mat4 get_inverse_projection_view() const noexcept;

	void set_field_of_vision(f32 fov_radians);
	_NODISCARD f32 get_field_of_vision() const;

	inline void set_fov(f32 const fov_radians) { set_field_of_vision(fov_radians); }
	inline _NODISCARD f32 get_fov() const { return get_field_of_vision(); }

	void set_aspect_ratio(f32 const aspect_ratio);
	_NODISCARD f32 get_aspect_ratio() const;

	void set_far_plane(f32 const far_plane);
	_NODISCARD f32 get_far_plane() const;

	void set_near_plane(f32 const near_plane);
	_NODISCARD f32 get_near_plane() const;

	void set_size(f32 const left, f32 const right, f32 const bottom, f32 const top);
	_NODISCARD vec4 get_size() const;

	void render_setup(RenderPassInfo const &info) override;
	void editor() override;

	void refresh_matrices();

	_NODISCARD Frustum get_frustum() const;

	_NODISCARD static Camera3D *get_current_camera_entity();

	void make_current();

protected:
	void update_view_matrix();
	void update_projection_matrix();

private:
	static RID current_camera_;
	
	union CameraAttributes {
		struct PerspectiveCameraAttributes {
			f32 fov_ = 90.0f;
			f32 aspect_ratio_ = 16.0f / 9.0f;
		} perspective_;
		struct OrthographicCameraAttributes {
			f32 left_ = 1.0f;
			f32 right_ = 1.0f;
			f32 bottom_ = 1.0f;
			f32 top_ = 1.0f;
		} orthographic_;
	} camera_attributes_;
	
	f32 near_z_ = 0.05f;
	f32 far_z_ = 4096.0f;
	
protected:
	RID camera_buffer_;
	RID camera_bind_group_;
	bool camera_bind_group_exists_ = false;
	mat4 view_;
	mat4 projection_;
	mat4 inverse_view_;
	mat4 inverse_projection_;
	
private:
	
	bool is_orthographic_;
	bool is_current_;
};
