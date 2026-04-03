#include "editor_camera.hpp"
#include "ecs/transform.h"
#include "engine/Input.h"
#include <glm/gtx/euler_angles.hpp>

ComponentProvider<EditorCamera3D> ComponentProvider<EditorCamera3D>::instance_ = ComponentProvider();

void EditorCamera3D::update(f64 const x) {
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
	
	transform.rotation = q0;
	mat4 rotation = glm::mat4_cast(q0);
	
	// Calculate right and up vectors
	vec3 const forward(rotation[0][2], rotation[1][2], rotation[2][2]);
	vec3 const right   = glm::normalize(glm::cross(forward, vec3(0.0f, 1.0f, 0.0f)));
	vec3 up      = glm::normalize(glm::cross(right, forward));

	if (Input::justPressed(win, KEY_Z)) {
		_STD cout << "just pressed!\n";
		if (captured_)
			glfwSetInputMode(win.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(win.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		captured_ = !captured_;
	}

	float speedMult = 10.0f;
	if (Input::pressed(win, KEY_LEFT_SHIFT)) {
		speedMult *= 100.0f;
	}

	if (Input::pressed(win, KEY_LEFT_CONTROL)) {
		speedMult *= 0.1f;
	}
	
	transform.translation += forward * input.y * speedMult * static_cast<f32>(x);
	transform.translation -=   right * input.x * speedMult * static_cast<f32>(x);
	
	transform.order = RotateTranslateScale;
	
	refreshMatrices();
}
void EditorCamera3D::mouse(MouseInputEvent const &event) {
	yawPitch += event.delta_relative * 1500.0f;
}
