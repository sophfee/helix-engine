#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "imgui.h"
#include "ecs/transform.h"

ComponentServer<Camera3D> ComponentServer<Camera3D>::instance_ = ComponentServer();
Camera3D *Camera3D::current_camera_ = nullptr;

Camera3D::Camera3D(SharedPtr<SceneTree> const &scene_tree, SharedPtr<Entity> const &ent) : Component(scene_tree, ent), camera_attributes_(), near_z_(0), far_z_(0), is_orthographic_(false), is_current_(false) {}

mat4 Camera3D::viewMatrix() const noexcept { return view_; }
mat4 Camera3D::inverseViewMatrix() const noexcept { return inverse_view_; }
mat4 Camera3D::projectionMatrix() const noexcept { return projection_; }
mat4 Camera3D::inverseProjectionMatrix() const noexcept { return inverse_projection_; }
mat4 Camera3D::projectionViewMatrix() const noexcept { return projection_ * view_; }
mat4 Camera3D::inverseProjectionViewMatrix() const noexcept { return inverse_projection_ * inverse_view_; }

void Camera3D::setFieldOfVision(f32 const fov_radians) {
	is_orthographic_ = false;
	camera_attributes_.perspective_.fov_ = fov_radians;
	updateProjectionMatrix();
}

f32 Camera3D::fieldOfVision() const {
	HELIX_ASSUME(!is_orthographic_, "Can't request parameters of a perspective based camera as an orthographic.")
	return camera_attributes_.perspective_.fov_;
}

void Camera3D::setAspectRatio(f32 const aspect_ratio) {
	is_orthographic_ = false;
	camera_attributes_.perspective_.aspect_ratio_ = aspect_ratio;
	updateProjectionMatrix();
}

f32 Camera3D::aspectRatio() const {
	HELIX_ASSUME(!is_orthographic_, "Can't request parameters of a perspective based camera as an orthographic.")
	return camera_attributes_.perspective_.aspect_ratio_;
}

void Camera3D::setFarPlane(f32 const far_plane) {
	far_z_ = far_plane;
	updateProjectionMatrix();
}

f32 Camera3D::farPlane() const {
	return far_z_;
}

void Camera3D::setNearPlane(f32 const near_plane) {
	near_z_ = near_plane;
	updateProjectionMatrix();
}
f32 Camera3D::nearPlane() const {
	return near_z_;
}

void Camera3D::setSize(f32 const left, f32 const right, f32 const bottom, f32 const top) {
	is_orthographic_ = true;
	camera_attributes_.orthographic_.left_ = left;
	camera_attributes_.orthographic_.right_ = right;
	camera_attributes_.orthographic_.bottom_ = bottom;
	camera_attributes_.orthographic_.top_ = top;
	updateProjectionMatrix();
}

vec4 Camera3D::size() const {
	return vec4(
		camera_attributes_.orthographic_.left_,
		camera_attributes_.orthographic_.right_,
		camera_attributes_.orthographic_.bottom_,
		camera_attributes_.orthographic_.top_
	);
}

void Camera3D::renderSetup(RenderPassInfo const &info) {
	if (info.pass != RenderPassType::Normal) return;
	//refreshMatrices();
	glUniformMatrix4fv(info.view_matrix_location, 1, GL_FALSE, glm::value_ptr(view_));
	glUniformMatrix4fv(info.projection_matrix_location, 1, GL_FALSE, glm::value_ptr(projection_));
}


void Camera3D::editor() {
	ImGui::Text("Perspective");
	ImGui::InputFloat("Field of Vision", &camera_attributes_.perspective_.fov_);
	ImGui::InputFloat("Aspect Ratio", &camera_attributes_.perspective_.aspect_ratio_);
	ImGui::Text("Clipping Plane");
	ImGui::InputFloat("Near Plane", &near_z_);
	ImGui::InputFloat("Far Plane", &far_z_);
}

void Camera3D::refreshMatrices() {
	updateViewMatrix();
	updateProjectionMatrix();
}

SharedPtr<Entity> Camera3D::currentCameraEntity() {
	if (current_camera_ == nullptr)
		return nullptr;
	
	return current_camera_->entity.lock();
}

void Camera3D::makeCurrent() {
	if (is_current_) return;

	if (current_camera_ != nullptr)
		current_camera_->is_current_ = false;

	current_camera_ = this;
	is_current_ = true;
}

void Camera3D::updateViewMatrix() {
	SharedPtr<Entity> const &owner = entity.lock();
	Transform const &transform = owner->component<Transform>();
	this->view_ = transform.matrix();
	this->inverse_view_ = glm::inverse(view_);
}

void Camera3D::updateProjectionMatrix() {
	if (is_orthographic_)
		projection_ = glm::ortho(
			camera_attributes_.orthographic_.left_,
			camera_attributes_.orthographic_.right_,
			camera_attributes_.orthographic_.bottom_,
			camera_attributes_.orthographic_.top_,
			near_z_, far_z_
		);
	else
		projection_ = glm::perspective(
			camera_attributes_.perspective_.fov_,
			camera_attributes_.perspective_.aspect_ratio_,
			near_z_, far_z_
		);
	
	inverse_projection_ = glm::inverse(projection_);
}
