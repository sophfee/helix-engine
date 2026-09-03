#include "camera.hpp"
#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "imgui.h"
#include "ecs/transform.h"
#include "ecs/core/scene_tree.hpp"
#include "engine/main-loop.hpp"
#include "gpu/driver.hpp"
#include "gpu/renderers/forward.hpp"

ComponentProvider<Camera3D> ComponentProvider<Camera3D>::instance_ = ComponentProvider();
Camera3D::Camera3D() : camera_attributes_(CameraAttributes::PerspectiveCameraAttributes{.fov_ = 90.0f, .aspect_ratio_ = 16.0f/9.0f}), near_z_(0), far_z_(0), is_orthographic_(false), is_current_(false) {
}

Camera3D::Camera3D(SharedPtr<SceneTree> const &scene_tree, const RID ent) : Component(scene_tree, ent), camera_attributes_(CameraAttributes::PerspectiveCameraAttributes{.fov_ = 90.0f, .aspect_ratio_ = 16.0f/9.0f}), near_z_(0), far_z_(0), is_orthographic_(false), is_current_(false) {}

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
	
	GraphicsBackend* driver = GraphicsDriver::get();
	//if (!camera_bind_group_exists_) {
	//	IRenderer* renderer = Main::renderer().value();
	//	ForwardRenderer* forward_renderer = (ForwardRenderer*)renderer;
	//
	//	camera_bind_group_ = driver->bind_group_create({
	//		.label = "EditorCamera3D CameraData Bind Group",
	//		.layout = forward_renderer->bind_group_layout,
	//		.entries = {
	//			BindGroupEntryDescriptor{
	//				.binding = 0,
	//				.resource = BindingResource(
	//					camera_buffer_,
	//					0,
	//					384
	//				)
	//			}
	//		}
	//	});
	//	camera_bind_group_exists_ = true;
	//}
	
	//const vk::DeviceAddress address = driver->buffer_virtual_address(camera_buffer_);
	
	//driver->set_bind_group(info.cmd, info.pipeline_layout, 0, camera_bind_group_);
	
	//driver->push_constants(info.cmd, info.pipeline_layout, PushConstantRangeDescriptor{
	//	.visibility = gfx::ShaderStage::eVertex,
	//	.offset = 0,
	//	.size = sizeof(vk::DeviceAddress),
	//}, &address);
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

Camera3D *Camera3D::currentCameraEntity() {
	if (!current_camera_.valid())
		return nullptr;
	
	return std::addressof(Main::mainLoop().value().renderer().value()->sceneTree()->entityMut(current_camera_)->component<Camera3D>());
}

void Camera3D::makeCurrent() {
	if (is_current_) return;

	if (current_camera_.valid())
		currentCameraEntity()->is_current_ = false;

	current_camera_ = this->entity()->id();
	is_current_ = true;
}

void Camera3D::updateViewMatrix() {
	const Entity* owner = entity();
	Transform const &transform = owner->component<Transform>();
	this->view_ = transform.matrix();
	this->inverse_view_ = glm::inverse(view_);
	//assert(glm::any(glm::isnan(view_[0])));
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
	else {
		near_z_ = std::max(near_z_, 0.05f);
		far_z_ = std::max(far_z_, near_z_ + 100.0f);
		projection_ = glm::perspective(
			camera_attributes_.perspective_.fov_,
			camera_attributes_.perspective_.aspect_ratio_,
			near_z_, far_z_
		);
	}
	//assert(glm::any(glm::isnan(projection_[0])));
	inverse_projection_ = glm::inverse(projection_);
}

RID Camera3D::current_camera_ = RID{0, 0};