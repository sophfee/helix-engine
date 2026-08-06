// ReSharper disable CppTooWideScopeInitStatement
#include "window.hpp"

#include "driver.hpp"
#include "engine/Input.h"
#include "GLFW/glfw3native.h"

Window::Window(
	ivec2 const &p_startingSize,
	_STD optional<_STD string> const &p_windowTitle,
	_STD optional<_STD reference_wrapper<Window>> const &p_sharedWindow,
	_STD optional<WindowConfig> const &p_config
) : Window() {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	bool bMakeFullscreen = false;
	
	window = glfwCreateWindow(p_startingSize.x, p_startingSize.y,
		p_windowTitle.has_value() ? p_windowTitle.value().c_str() : "New Window", nullptr,
		p_sharedWindow.has_value() ? p_sharedWindow.value().get().window : nullptr);
	assert(window);
	
	glfwSetErrorCallback([](int error_code, const char *description) {
		printf("GLFW Error [%d]: %s\n", error_code, description);
	});
	
	if (bMakeFullscreen) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		ivec2 size;
		glfwGetMonitorPhysicalSize(monitor, &size.x, &size.y);
	}

	Input::installCallbacks(*this);
}

Window::Window() {
}

Window::~Window() {
	auto* driver = GraphicsDriver::get();
	
	driver->surface_delete(surface);
	glfwDestroyWindow(window);
}

void Window::createSurface(bool create_depth_buffer, Optional<gfx::Format> target_color_format,
	Optional<gfx::ColorSpace> target_color_space, Optional<gfx::PresentMethod> target_present_mode, 
	Optional<gfx::Format> target_depth_format) {
	GraphicsBackend *driver = GraphicsDriver::get();

	const gfx::Format depth_image_format = target_depth_format.value_or(gfx::Format::eDepth32SfloatStencil8Uint);
	surface = driver->surface_create(this, SurfaceDescriptor{
		.format = target_color_format,
		.usage = gfx::ImageUsage::eColorAttachment,
		.present_method = target_present_mode,
		.color_space = target_color_space
	});

	if (create_depth_buffer) {
		VkExtent2D window_extent{
			.width = static_cast<uint32_t>(getSize().x),
			.height = static_cast<uint32_t>(getSize().y)
		};

		const ImageDescriptor depth_image_create_desc = {
			.label = "Depth Attachment",
			.format = depth_image_format,
			.type = gfx::ImageType::e2D,
			.usage = gfx::ImageUsage::eDepthStencilAttachment,
			.samples = gfx::SampleCount::e1,
			.memory_usage = gfx::MemoryUsage::eAuto,
			.allocation_hints = gfx::AllocationHint::eDedicated,
			.size = { window_extent.width, window_extent.height, 1 },
			.array_layers = 1,
			.mip_levels = 1
		};
		depth_image = driver->image_create(depth_image_create_desc);

		const ImageViewDescriptor depth_image_view_descriptor {
			.image = depth_image,
			.type = gfx::ImageViewType::e2D,
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = gfx::Aspect::eDepth
			}
		};
		depth_image_view = driver->image_view_create(depth_image_view_descriptor);
	}
	
	has_swapchain = true;
}

SharedPtr<IRenderer> Window::renderer() const {
	return renderer_;
}

void Window::setRenderer(SharedPtr<IRenderer> const &renderer) {
	renderer_ = renderer;
}

ivec2 Window::getSize() const {
	if (!window) return { 1, 1 };
	ivec2 size;
	glfwGetWindowSize(window, &size.x, &size.y);
	return size;
}

void Window::setSize(ivec2 const &size) const {
	glfwSetWindowSize(window, size.x, size.y);
}

ivec4 Window::viewport() const {
	return {0, 0, extent.width, extent.height};
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(window);
}

void Window::setSceneTree(SharedPtr<SceneTree> const &scene_tree) {
	scene_tree_ = scene_tree;
}

SharedPtr<SceneTree> const & Window::sceneTree() const {
	return scene_tree_;
}

void Window::hide() const {
	glfwHideWindow(window);
}

void Window::show() const {
	glfwShowWindow(window);
	GraphicsBackend* driver = GraphicsDriver::get();
	driver->force_wait_for_device_idle();
}

void Window::setVisible(bool const visible) const {
	if (visible) show(); else hide();
}

bool Window::visible() const {
	return glfwGetWindowAttrib(window, GLFW_VISIBLE) == GLFW_TRUE;
}

void Window::setFramebufferSizeCallback(GLFWframebuffersizefun const fun) const {
	glfwSetFramebufferSizeCallback(window, fun);
}

void Window::makeContextCurrent() const {
	//glfwMakeContextCurrent(window);
}
void Window::swapBuffers() const {
	//glfwSwapBuffers(window);
}

void Window::dispose() {
	//if (!renderer_->disposed())
	//	renderer_->dispose();

	GraphicsBackend *driver = GraphicsDriver::get();
	driver->image_view_delete(depth_image_view);
	driver->image_delete(depth_image);
	driver->surface_delete(surface);
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool Window::disposed() const {
	return window == nullptr;
}