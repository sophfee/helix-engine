#include "editor_camera.hpp"
#include "ecs/transform.h"
#include <glm/gtx/euler_angles.hpp>

#include "gpu/driver.hpp"
#include "gpu/window.hpp"
#include "gpu/renderers/forward.hpp"

ComponentProvider<EditorCamera3D> ComponentProvider<EditorCamera3D>::instance_ = ComponentProvider();

struct CameraData {
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
	mat4 inverseView;
	mat4 inverseProjection;
	mat4 inverseViewProjection;
};

EditorCamera3D::EditorCamera3D() : Camera3D() {
}

EditorCamera3D::EditorCamera3D(SharedPtr<SceneTree> const &scene_tree, const RID ent): Camera3D(scene_tree, ent) {
	IGpuDriver *driver = GraphicsSystem::get_driver();

	//const BufferDescriptor buffer_create_desc = {
	//	.label = "EditorCamera3D CameraData Buffer",
	//	.size = sizeof(CameraData),
	//	.usage = BitFlag(gfx::BufferUsage::eUniform) | gfx::BufferUsage::eShaderDeviceAddress,
	//	.memory_usage = gfx::MemoryUsage::eAuto,
	//	.allocation_hints = BitFlag(gfx::AllocationHint::eMapped) | gfx::AllocationHint::eHostSequentialWrite | gfx::AllocationHint::eAllowTransferInstead
	//};
	//
	//camera_buffer_ = driver->buffer_create(buffer_create_desc);
	//driver->buffer_set_name(camera_buffer_, "CAMERA BUFFER");
	
	make_current();
}
void EditorCamera3D::update(f64 const delta_time) {
	Window &win = *get_window();
	vec2 input = win.axis2(eD, eA, eW, eS);
	// Sensitivity
	input *= 0.10f;
	// Calculate forward vector
	Entity* owner = get_entity();
	Transform &transform = owner->get_component<Transform>();
	// Get mouse delta
	if (captured_) {
		vec2 const mouse_delta = win.get_mouse_delta();
		yaw_pitch_.x -= mouse_delta.x * 0.1f;
		yaw_pitch_.x = glm::mod(yaw_pitch_.x, 360.0f);
		yaw_pitch_.y -= mouse_delta.y * 0.1f;
		yaw_pitch_.y = glm::mod(yaw_pitch_.y, 360.0f);
		
		if (abs(mouse_delta.x) > 0.001f || abs(mouse_delta.y) > 0.001f) {
			transform.dirty_[0] = true;
			transform.dirty_[1] = true;
		}
	}
	// Move the camera
	quat const q1(1.0f, 0.0f, 0.0f, 0.0f);
	quat const q2 = glm::rotate(q1, glm::radians(yaw_pitch_.y), vec3(1.0f, 0.0f, 0.0f));
	quat const q0 = glm::rotate(q2, glm::radians(yaw_pitch_.x), vec3(0.0f, 1.0f, 0.0f));
	transform.rotation  =  q0;
	mat4      rotation  =  glm::mat4_cast(q0);
	// Calculate right and up vectors
	vec3 const forward(rotation[0][2], rotation[1][2], rotation[2][2]);
	vec3 const right   = glm::normalize(glm::cross(forward, vec3(0.0f, 1.0f, 0.0f)));
	vec3       up      = glm::normalize(glm::cross(right, forward));

	if (win.just_pressed(eZ)) {
		if (captured_)
			win.set_mouse_capture_mode(MouseCapture::eNone);
		else
			win.set_mouse_capture_mode(MouseCapture::eCaptured);
		captured_ = !captured_;
	}

	float speedMult = 50.0f;
	if (win.pressed(eLeftShift))
		speedMult = 250.0f;
	if (win.pressed(eLeftControl))
		speedMult = 12.0f;

	transform.translation += forward * input.y * speedMult * static_cast<f32>(delta_time);
	transform.translation -=   right * input.x * speedMult * static_cast<f32>(delta_time);
	transform.order = RotateTranslateScale;
	
	if (abs(input.x) > 0.001f || abs(input.y) > 0.001f) {
		transform.dirty_[0] = true;
		transform.dirty_[1] = true;
	}

	//CameraData* camera_data = (CameraData*)GraphicsDriver::get()->buffer_mapped_data(camera_buffer_);
	//camera_data->view = viewMatrix();
	//camera_data->inverseView = inverseViewMatrix();
	//camera_data->projection = projectionMatrix();
	//camera_data->inverseProjection = inverseProjectionMatrix();
	//camera_data->viewProjection = projectionViewMatrix();
	//camera_data->inverseViewProjection = inverseProjectionViewMatrix();
}

void EditorCamera3D::mouse(MouseInputEvent const &event) {
}

void EditorCamera3D::destroy() {
	//GraphicsDriver::get()->buffer_delete(camera_buffer_);
}
