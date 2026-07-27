#include "editor_camera.hpp"
#include "ecs/transform.h"
#include "engine/Input.h"
#include <glm/gtx/euler_angles.hpp>

#include "gpu/driver.hpp"
#include "gpu/window.hpp"

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
	
	GraphicsDriver* driver = GraphicsDriver::singleton();
	
	vk::BufferCreateInfo buffer_create_info = vk::BufferCreateInfo()
		.setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress)
		.setSize(sizeof(CameraData));

	const VmaAllocationCreateInfo allocation_create_info = {
		.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO
	};
	
	camera_buffer_ = driver->buffer_create(buffer_create_info, allocation_create_info);
	driver->buffer_set_allocation_name(camera_buffer_, "CAMERA BUFFER");
	
	makeCurrent();
}
void EditorCamera3D::update(f64 const delta_time) {
	Window const &win = *window();
	vec2 input = Input::vector(win, KEY_A, KEY_D, KEY_W, KEY_S);
	vec2 const mouse_delta = Input::mouseDelta();
	
	// Sensitivity
	input *= 1.0f;
	
	// Update yaw and pitch
	//yawPitch.y = glm::clamp(yawPitch.y, -89.0f, 89.0f);
	
	// Calculate forward vector
	
	SharedPtr<Entity> const &owner = entity.lock();
	Transform &transform = owner->component<Transform>();

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

	if (Input::justPressed(win, KEY_Z)) {
		if (captured_)
			glfwSetInputMode(win.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(win.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		captured_ = !captured_;
	}

	float speedMult = 50.0f;
	if (Input::pressed(win, KEY_LEFT_SHIFT)) {
		speedMult = 250.0f;
	}

	if (Input::pressed(win, KEY_LEFT_CONTROL)) {
		speedMult = 12.0f;
	}
	
	transform.translation += forward * input.y * speedMult * static_cast<f32>(delta_time);
	transform.translation -=   right * input.x * speedMult * static_cast<f32>(delta_time);
	
	transform.order = RotateTranslateScale;
	
	refreshMatrices();
	
	CameraData* camera_data = (CameraData*)GraphicsDriver::singleton()->buffer_get_mapped_address(camera_buffer_);
	camera_data->view = viewMatrix();
	camera_data->inverseView = inverseViewMatrix();
	camera_data->projection = projectionMatrix();
	camera_data->inverseProjection = inverseProjectionMatrix();
	camera_data->viewProjection = projectionViewMatrix();
	camera_data->inverseViewProjection = inverseProjectionViewMatrix();
}

void EditorCamera3D::mouse(MouseInputEvent const &event) {
	if (captured_)
		yawPitch += event.delta_relative * 1500.0f;
}

