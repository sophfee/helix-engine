// ReSharper disable CppTooWideScopeInitStatement
#include "driver.hpp"
#include "backends/vulkan_backend.hpp"
#include "backends/dx12_backend.hpp"

#include <glfw/glfw3.h>
#include "gltf.h"
#include "window.hpp"

//#define VMA_LEAK_LOG_FORMAT(format, ...) do { \
//        printf((format), __VA_ARGS__); \
//        printf("\n"); \
//    } while(false)
//
//#define VMA_DEBUG_LOG_FORMAT(format, ...) do { \
//       printf((format), __VA_ARGS__); \
//       printf("\n"); \
//   } while(false)

#define VMA_DEBUG_INITIALIZE_ALLOCATIONS 1
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

RID GraphicsBackend::_make_rid(ResourceKind kind, u32 slot) {
	return {slot, 0u};
}

GraphicsDriver::GraphicsDriver(const RenderingApiBackend backend) : backend_api_(backend) {
}

GraphicsDriver::~GraphicsDriver() {
	stop();
}

void GraphicsDriver::start() {
	if (backend_api_ != RenderingApiBackend::eVulkan) {
		assert(false && "Only Vulkan backend bootstrap is currently implemented");
	}
	
	switch (backend_api_) {
	case RenderingApiBackend::eVulkan:
		if (backend_ != nullptr && dynamic_cast<VkGraphicsBackend*>(backend_.get())) {
			// Already initialized with Vulkan backend
			return;
		}
		backend_ = std::make_unique<VkGraphicsBackend>();
		backend_->initialize();
		break;
	default:
		assert(false && "Unsupported backend");
	}
}

void GraphicsDriver::stop() {
	backend_.reset();
}

void GraphicsDriver::set_backend(const RenderingApiBackend backend) {
	backend_api_ = backend;
	start();
}

GraphicsDriver * GraphicsDriver::singleton() {
	static GraphicsDriver instance;
	return &instance;
}

GraphicsBackend * GraphicsDriver::get() {
	return singleton()->backend_.get();
}