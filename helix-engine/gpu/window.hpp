#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "geometry.hpp"
#include "types.hpp"
#include "engine/disposable.hpp"
#include "engine/rid.hpp"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"

class GraphicsDriver;
class IRenderer;
class SceneTree;

struct VideoMode {
	int red_bits;
	int green_bits;
	int blue_bits;
	int refresh_rate;
};

struct WindowConfig {
	bool transparent;
	bool resizable;
	bool fullscreen;
	bool decorated;
	_STD optional<VideoMode> videoMode;
};

class Window : public IDisposable {
	SharedPtr<SceneTree> scene_tree_;
	SharedPtr<IRenderer> renderer_;
	
public:
	vk::SurfaceKHR surface_khr;
	vk::SwapchainKHR swapchain_khr;
	Vec<vk::Image> swapchain_images;
	Vec<RID> swapchain_image_views;
	
	RID depth_image;
	RID depth_image_view;
	
	vk::Extent2D extent;
private:
	static vk::Format getFormatFromColorSpace(vk::SurfaceKHR surface_khr, vk::ColorSpaceKHR const &colorSpace);
	static vk::ColorSpaceKHR getColorSpaceFromFormat(vk::SurfaceKHR surface_khr, vk::Format const &format);
public:
	vk::Format depth_format = vk::Format::eUndefined;
	vk::Format color_format = vk::Format::eUndefined;
	vk::ColorSpaceKHR color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
	vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo; // Fifo is available on every single platform.
	
	static constexpr auto framesInFlight = 2;
	Array<RID, framesInFlight> graphics_command_buffers;
	Array<RID, framesInFlight> render_finished_semaphores;
	Array<RID, framesInFlight> image_available_semaphores;
	Array<RID, framesInFlight> graphics_fences;
	mutable u32 frame_index = 0;
	u32 image_index = 0;
	GLFWwindow *window;
	
private:
	bool has_depth_attachment = false;
	bool has_swapchain = false;
	bool has_command_buffers = false;
public:
	
	Window();
	Window(
		ivec2 const &p_startingSize,
		_STD optional<_STD string> const &p_windowTitle = _STD nullopt,
		_STD optional<_STD reference_wrapper<Window>> const &p_sharedWindow = _STD nullopt,
		_STD optional<WindowConfig> const &p_config = _STD nullopt
	);
	~Window() override;
	
	vk::SurfaceKHR createSurface();
	void createSwapchain(
		bool create_depth_buffer = false,
		Optional<vk::Format> target_color_format = std::nullopt,
		Optional<vk::ColorSpaceKHR> target_color_space = std::nullopt,
		Optional<vk::PresentModeKHR> target_present_mode = std::nullopt,
		Optional<vk::Format> target_depth_format = std::nullopt
	);

	[[nodiscard]] vk::SurfaceKHR surface() const;

	// no copy no move
	Window(Window const& window) = delete;
	Window(Window&& window) = delete;
	Window& operator=(Window const& window) = delete;
	Window& operator=(Window&& window) = delete;

	[[nodiscard]] SharedPtr<IRenderer> renderer() const;
	void setRenderer(SharedPtr<IRenderer> const& renderer);

	_NODISCARD ivec2 getSize() const;
	void setSize(ivec2 const& size) const;

	_NODISCARD ivec4 viewport() const;

	_NODISCARD bool shouldClose() const;

	void setSceneTree(SharedPtr<SceneTree> const& scene_tree);
	_NODISCARD SharedPtr<SceneTree> const& sceneTree() const;

	void hide() const;
	void show() const;
	void setVisible(bool visible) const;
	_NODISCARD bool visible() const;

	void setFramebufferSizeCallback(GLFWframebuffersizefun fun) const;

	void setFramebufferSizeCallback(auto fun) const {
		glfwSetFramebufferSizeCallback(window, fun);
	}

	void makeContextCurrent() const;
	void swapBuffers() const;
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
	
	friend class GraphicsDriver;
};