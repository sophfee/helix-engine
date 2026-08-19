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

EditorCamera3D::EditorCamera3D(SharedPtr<SceneTree> const &scene_tree, SharedPtr<Entity> const &ent): Camera3D(scene_tree, ent) {
	GraphicsBackend *driver = GraphicsDriver::get();

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
	
	makeCurrent();
}
void EditorCamera3D::update(f64 const delta_time) {
	Window &win = *window();
	vec2 input = win.axis2(eD, eA, eW, eS);
	// Sensitivity
	input *= 0.10f;
	// Calculate forward vector
	SharedPtr<Entity> const &owner = entity.lock();
	Transform &transform = owner->component<Transform>();
	// Get mouse delta
	if (captured_) {
		vec2 const mouse_delta = win.mouseDelta();
		yawPitch.x -= mouse_delta.x * 0.1f;
		yawPitch.x = glm::mod(yawPitch.x, 360.0f);
		yawPitch.y -= mouse_delta.y * 0.1f;
		yawPitch.y = glm::mod(yawPitch.y, 360.0f);
	}
	// Move the camera
	quat const q1(1.0f, 0.0f, 0.0f, 0.0f);
	quat const q2 = glm::rotate(q1, glm::radians(yawPitch.y), vec3(1.0f, 0.0f, 0.0f));
	quat const q0 = glm::rotate(q2, glm::radians(yawPitch.x), vec3(0.0f, 1.0f, 0.0f));
	transform.rotation  =  q0;
	mat4      rotation  =  glm::mat4_cast(q0);
	// Calculate right and up vectors
	vec3 const forward(rotation[0][2], rotation[1][2], rotation[2][2]);
	vec3 const right   = glm::normalize(glm::cross(forward, vec3(0.0f, 1.0f, 0.0f)));
	vec3       up      = glm::normalize(glm::cross(right, forward));

	if (win.justPressed(eZ)) {
		if (captured_)
			win.setMouseCaptureMode(MouseCapture::eNone);
		else
			win.setMouseCaptureMode(MouseCapture::eCaptured);
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
	
	refreshMatrices();
	
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
