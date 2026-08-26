// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
// ReSharper disable CppCStyleCast

#include "graphics.hpp"

#include <chrono>
#include <filesystem>
#include <Windows.h>
#include "driver.hpp"
#include "glfw/glfw3.h"
#include "khr/ktx.h"

void initGraphics() {
	HELIX_ASSUME(glfwInit() == GLFW_TRUE, "GLFW3 failed to initialize");
	GraphicsDriver::singleton()->Start();
}

void terminateGraphics() {
	GraphicsDriver::singleton()->Stop();
	glfwTerminate();
}