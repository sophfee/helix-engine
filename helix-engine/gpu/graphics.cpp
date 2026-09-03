// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
// ReSharper disable CppCStyleCast

#include "graphics.hpp"

#include <chrono>
#include <filesystem>
#include <Windows.h>
#include "driver.hpp"
#include "glfw/glfw3.h"
#include "khr/ktx.h"

void initialize_graphics() {
	HELIX_ASSUME(glfwInit() == GLFW_TRUE, "GLFW3 failed to initialize");
	GraphicsDriver::singleton()->init();
}

void shutdown_graphics() {
	GraphicsDriver::singleton()->shutdown();
	glfwTerminate();
}