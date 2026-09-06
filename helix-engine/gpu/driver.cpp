// ReSharper disable CppTooWideScopeInitStatement
#include "driver.hpp"
#include "backends/vulkan_backend.hpp"
#include "backends/dx12_backend.hpp"

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

bool IGpuDriver::is_valid_rid(const RID rid) {
	return rid.lower > 0;
}

RID IGpuDriver::_make_rid(ResourceKind kind, u32 slot) {
	return {slot, 0u};
}

GraphicsSystem::GraphicsSystem(const RenderingApiBackend backend) : backend_api_(backend) {
}

GraphicsSystem::~GraphicsSystem() {
	shutdown();
}

void GraphicsSystem::init() {
	if (backend_api_ != RenderingApiBackend::eVulkan) {
		assert(false && "Only Vulkan backend bootstrap is currently implemented");
	}
	
	switch (backend_api_) {
	case RenderingApiBackend::eVulkan:
		if (backend_ != nullptr && dynamic_cast<VkGraphicsDriverBackend*>(backend_.get())) {
			// Already initialized with Vulkan backend
			return;
		}
		backend_ = std::make_unique<VkGraphicsDriverBackend>();
		backend_->initialize();
		break;
	default:
		assert(false && "Unsupported backend");
	}
}

void GraphicsSystem::shutdown() {
	backend_.reset();
}

void GraphicsSystem::set_backend(const RenderingApiBackend backend) {
	backend_api_ = backend;
	init();
}

GraphicsSystem * GraphicsSystem::get_singleton() {
	static GraphicsSystem instance;
	return &instance;
}

IGpuDriver * GraphicsSystem::get_driver() {
	return get_singleton()->backend_.get();
}