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
	GraphicsDriver* driver = GraphicsDriver::singleton();
	for (auto i = 0; i < framesInFlight; ++i) {
		image_available_semaphores[i] = driver->semaphore_create();
		render_finished_semaphores[i] = driver->semaphore_create();
		graphics_fences[i] = driver->fence_create(true);
	}
}

Window::~Window() {
	auto* driver = GraphicsDriver::singleton();
	
	for (auto i = 0; i < framesInFlight; ++i) {
		driver->semaphore_delete(image_available_semaphores[i]);
		driver->semaphore_delete(render_finished_semaphores[i]);
		driver->fence_delete(graphics_fences[i]);
	}
	
	std::erase(driver->windows, this);
	glfwDestroyWindow(window);
}

vk::SurfaceKHR Window::createSurface() {
	VkSurfaceKHR surfaceKhr;
	if (glfwCreateWindowSurface(GraphicsDriver::singleton()->instance, window, NULL, &surfaceKhr) != VK_SUCCESS) {
		terminate();
	}
	surface_khr = surfaceKhr;
	return surface_khr;
}

vk::Format Window::getFormatFromColorSpace(const vk::SurfaceKHR surface_khr, vk::ColorSpaceKHR const &colorSpace) {
	const std::vector<vk::SurfaceFormatKHR> surface_formats = 
		GraphicsDriver::singleton()->adapter.getSurfaceFormatsKHR(surface_khr);
	for (vk::SurfaceFormatKHR const &surface_format : surface_formats)
		if (surface_format.colorSpace == colorSpace)
			return surface_format.format;

	return vk::Format::eUndefined;
}

vk::ColorSpaceKHR Window::getColorSpaceFromFormat(vk::SurfaceKHR surface_khr, vk::Format const &format) {
	const std::vector<vk::SurfaceFormatKHR> surface_formats = 
		GraphicsDriver::singleton()->adapter.getSurfaceFormatsKHR(surface_khr);
	
	for (vk::SurfaceFormatKHR const &surface_format : surface_formats)
		if (surface_format.format == format)
			return surface_format.colorSpace;

	return vk::ColorSpaceKHR::eSrgbNonlinear;
}

static  Vec<vk::Format> optimal_depth_formats = {
	vk::Format::eD32SfloatS8Uint,
	vk::Format::eD24UnormS8Uint,
	vk::Format::eD16UnormS8Uint
};

constexpr std::array<std::tuple<vk::PresentModeKHR, i32>, 7> present_mode_rankings = {
	std::make_tuple(vk::PresentModeKHR::eImmediate, 10),
	std::make_tuple(vk::PresentModeKHR::eMailbox, 100),
	std::make_tuple(vk::PresentModeKHR::eFifo, 20),
	std::make_tuple(vk::PresentModeKHR::eFifoRelaxed, 25)
};

void Window::createSwapchain(bool create_depth_buffer,
	Optional<vk::Format> target_color_format, Optional<vk::ColorSpaceKHR> target_color_space,
	Optional<vk::PresentModeKHR> target_present_mode, Optional<vk::Format> target_depth_format) {
	
	GraphicsDriver *driver = GraphicsDriver::singleton();

	const vk::SurfaceCapabilitiesKHR surface_capabilities = driver->adapter.getSurfaceCapabilitiesKHR(surface_khr);
	extent = surface_capabilities.currentExtent;
	
	// Choose a presentation mode.
	i32 current_presentation_mode_score = 20;
	if (!target_present_mode.has_value()) {
		const std::vector<vk::PresentModeKHR> present_modes = driver->adapter.getSurfacePresentModesKHR(surface_khr);
		present_mode = vk::PresentModeKHR::eFifo;
		
		for (vk::PresentModeKHR mode : present_modes)
			if (present_mode != mode)
				for (auto& tuple : present_mode_rankings)
					if (std::get<1>(tuple) > current_presentation_mode_score)
						present_mode = std::get<0>(tuple);
	}
	else
		present_mode = target_present_mode.value();

	// Choose a Format + Colorspace
	if (target_color_space.has_value()) {
		color_space = target_color_space.value();
		color_format = target_color_format.value_or(
			getFormatFromColorSpace(surface_khr, color_space)
		);
	}
	else if (target_color_format.has_value()) {
		color_format = target_color_format.value();
		color_space = getColorSpaceFromFormat(surface_khr, color_format);
	}
	else {
		// If nothing is specified, just fallback to some safe options.
		color_format = vk::Format::eB8G8R8A8Srgb;
		color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
	}
	
	// Create the swapchain:
	vk::SwapchainCreateInfoKHR swapchain_create_info = vk::SwapchainCreateInfoKHR()
		.setSurface(surface_khr)
		.setMinImageCount(surface_capabilities.minImageCount)
		.setImageFormat(color_format)
		.setImageColorSpace(color_space)
		.setImageExtent(extent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(present_mode);
	swapchain_khr = driver->device.createSwapchainKHR(swapchain_create_info);
	
	// Get Swapchain Images, make ImageViews for each.
	vk::ImageViewCreateInfo swapchain_image_view_create_info = vk::ImageViewCreateInfo()
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(color_format)
		.setSubresourceRange(vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setLayerCount(1)
			.setLevelCount(1));
	
	swapchain_images = driver->device.getSwapchainImagesKHR(swapchain_khr);
	swapchain_image_views.resize(swapchain_images.size());
	
	for (auto frame = 0; frame < swapchain_images.size(); ++frame) {
		swapchain_image_view_create_info.setImage(swapchain_images[frame]);
		swapchain_image_views[frame] = driver->image_view_create(swapchain_image_view_create_info);
	}
	
	// Depth Attachment
	if (!create_depth_buffer)
		return;

	for (const vk::Format optimal_format : optimal_depth_formats) {
		vk::FormatProperties format_properties = driver->adapter.getFormatProperties(optimal_format);
		if (format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
			depth_format = optimal_format;
			break;
		}
	}
	
	assert(depth_format != vk::Format::eUndefined);
	
	vk::ImageCreateInfo depth_image_create_info = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setFormat(depth_format)
		.setExtent(vk::Extent3D(extent, 1))
		.setMipLevels(1)
		.setArrayLayers(1)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setTiling(vk::ImageTiling::eOptimal)
		.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
		.setInitialLayout(vk::ImageLayout::eUndefined);
	
	VmaAllocationCreateInfo depth_image_allocation_create_info{
		.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
	};
	
	depth_image = driver->image_create(depth_image_create_info, depth_image_allocation_create_info);
	
	vk::ImageViewCreateInfo depth_image_view_create_info = vk::ImageViewCreateInfo()
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(depth_format)
		.setSubresourceRange(vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eDepth)
			.setLayerCount(1)
			.setLevelCount(1));
	
	depth_image_view = driver->image_view_create(depth_image, depth_image_view_create_info);
}

vk::SurfaceKHR Window::surface() const {
	return surface_khr;
}

SharedPtr<IRenderer> Window::renderer() const {
	return renderer_;
}

void Window::setRenderer(SharedPtr<IRenderer> const &renderer) {
	renderer_ = renderer;
}

ivec2 Window::getSize() const {
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

	GraphicsDriver *driver = GraphicsDriver::singleton();
	
	for (auto frame = 0; frame < framesInFlight; ++frame) {
		driver->semaphore_delete(render_finished_semaphores[frame]);
		driver->semaphore_delete(image_available_semaphores[frame]);
		driver->fence_delete(graphics_fences[frame]);
	}
	
	if (has_depth_attachment) {
		driver->image_view_delete(depth_image_view);
		driver->image_delete(depth_image);
	}
	
	if (has_swapchain) {
		for (const RID swapchain_image_view : swapchain_image_views)
			driver->image_view_delete(swapchain_image_view);
		
		driver->device.destroySwapchainKHR(swapchain_khr);
	}
	
	driver->instance.destroySurfaceKHR(surface_khr);
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool Window::disposed() const {
	return window == nullptr;
}