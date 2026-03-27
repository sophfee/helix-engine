// ReSharper disable CppCStyleCast
#include "Input.h"

#include <ranges>

Input::InputServerData Input::var = {};

void detail::callbackKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
	auto const engine_window_data = static_cast<GlfwWindowUserPointerEngineData *>(glfwGetWindowUserPointer(window));
	switch (static_cast<InputAction>(action)) {
		case InputAction::Press:
			engine_window_data->justPressed[static_cast<KeyCode>(key)] = true;
			break;
		case InputAction::Release:
			engine_window_data->justReleased[static_cast<KeyCode>(key)] = true;
			break;
		case InputAction::Repeat:
			// Do nothing for now.
			break;
	}
}

void detail::callbackCursorPosition(GLFWwindow *window, double x_pos, double y_pos) {
	auto const engine_window_data = static_cast<GlfwWindowUserPointerEngineData *>(glfwGetWindowUserPointer(window));
	auto const win = engine_window_data->pWindow;
	ivec2 const size = win->getSize();
	vec2 const lastKnownMousePosition = engine_window_data->lastMouseCoord;
	vec2 const newMousePosition(
		static_cast<float>(x_pos) / static_cast<float>(size.x),
		static_cast<float>(y_pos / static_cast<float>(size.y))
	);

	vec2 const sizef(static_cast<f32>(size.x), static_cast<f32>(size.y));

	if (newMousePosition.x == .5f && newMousePosition.y == .5f)
		return;

	printf("Mouse moved: %f, %f\n", newMousePosition.x, newMousePosition.y);
	Input::setMouseDelta(newMousePosition - lastKnownMousePosition);
	engine_window_data->lastMouseCoord = newMousePosition;
}

void detail::callbackCursorEnter(GLFWwindow *window, int entered) {}
void detail::callbackMouseButton(GLFWwindow *window, int button, int action, int mods) {}
void detail::callbackScroll(GLFWwindow *window, double x_offset, double y_offset) {}
void detail::callbackCharInput(GLFWwindow *window, unsigned int codepoint) {}
void detail::callbackFramebufferResize(GLFWwindow *window, int width, int height) {}
void detail::callbackWindowClose(GLFWwindow *window) {
	auto const engine_window_data = static_cast<GlfwWindowUserPointerEngineData *>(glfwGetWindowUserPointer(window));
	delete engine_window_data;
}
void detail::callbackWindowFocus(GLFWwindow *window, int focused) {}
void detail::callbackWindowMove(GLFWwindow *window, int width, int height) {}

bool Input::pressed(Window const &window, KeyCode key) {
	return glfwGetKey(window.window, static_cast<int>(key)) == GLFW_PRESS;
}

bool Input::justPressed(Window const &window, KeyCode key) {
	auto const engine_window_data = static_cast<GlfwWindowUserPointerEngineData *>(glfwGetWindowUserPointer(window.window));
	return engine_window_data->justPressed[key];
}

vec2 Input::vector(Window const &window, KeyCode const x_negative, KeyCode const x_positive, KeyCode const y_negative, KeyCode const y_positive) {
	return vec2(
		(pressed(window, x_negative) ? 1.0f : 0.0f) - (pressed(window, x_positive) ? 1.0f : 0.0f),
		(pressed(window, y_negative) ? 1.0f : 0.0f) - (pressed(window, y_positive) ? 1.0f : 0.0f)
	);
}

vec2 Input::mousePosition(Window const &window) {
	double x, y;
	glfwGetCursorPos(window.window, &x, &y);
	return vec2(
		static_cast<float>(x),
		static_cast<float>(y)
	);
}

vec2 Input::mouseDelta() {
	return var.mouseDelta;
}

void Input::process(Window const &window) {
	auto const engine_window_data = static_cast<GlfwWindowUserPointerEngineData *>(glfwGetWindowUserPointer(window.window));
	for (KeyCode const key : engine_window_data->justPressed | std::views::keys) {
		engine_window_data->justPressed[key] = false;
	}
	for (KeyCode const key : engine_window_data->justReleased | std::views::keys) {
		engine_window_data->justReleased[key] = false;
	}
	var.mouseDelta = vec2(0.0f);
}

void Input::setMouseDelta(vec2 const &delta) {
	var.mouseDelta = delta;
}

void Input::installCallbacks(Window const &window) {
	GLFWwindow *windowHandle = window.window;

	auto const data = new GlfwWindowUserPointerEngineData{
		.pWindow = const_cast<Window*>(&window),
		.justPressed = {},
		.justReleased = {}
	};
	
	glfwSetWindowUserPointer		(windowHandle,	(void*)data);
	glfwSetKeyCallback				(windowHandle,	detail::callbackKey);
	glfwSetCursorPosCallback		(windowHandle,	detail::callbackCursorPosition);
	glfwSetMouseButtonCallback		(windowHandle,	detail::callbackMouseButton);
	glfwSetCursorEnterCallback		(windowHandle,	detail::callbackCursorEnter);
	glfwSetScrollCallback			(windowHandle,	detail::callbackScroll);
	glfwSetCharCallback				(windowHandle,	detail::callbackCharInput);
	glfwSetFramebufferSizeCallback	(windowHandle,	detail::callbackFramebufferResize);
	glfwSetWindowCloseCallback		(windowHandle,	detail::callbackWindowClose);
	glfwSetWindowFocusCallback		(windowHandle,	detail::callbackWindowFocus);
	glfwSetWindowPosCallback		(windowHandle,	detail::callbackWindowMove);
}