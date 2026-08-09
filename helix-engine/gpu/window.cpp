// ReSharper disable CppTooWideScopeInitStatement
#include "window.hpp"

#include <ranges>

#include "driver.hpp"
#include "ecs/core/scene_tree.hpp"

#ifdef WINDOW_DRIVER_SDL2
#include <SDL2/SDL_vulkan.h>

SDL2Window::SDL2Window() {
}

SDL2Window::~SDL2Window() {
	if (!SDL2Window::disposed())
		SDL2Window::dispose();
}

void SDL2Window::dispose() {
	GraphicsBackend* r = GraphicsDriver::get();
	r->image_view_delete(depth_image_view);
	r->image_delete(depth_image);
	r->surface_delete(surface_);
	SDL_DestroyWindowSurface(window);
	SDL_DestroyWindow(window);
}

bool SDL2Window::disposed() const {
	return window == nullptr;
}

void SDL2Window::create(const RenderingApiBackend api, const ivec2 &starting_size, const std::optional<std::string> &title,
	const std::optional<IWindow *> &shared, const std::optional<WindowConfig> &config) {
	u32 flags = 0;
	
	if (config.has_value()) {
		const WindowConfig &config_value = config.value();
		if (config_value.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
		if (config_value.resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (!config_value.decorated) flags |= SDL_WINDOW_BORDERLESS;
	}
	
#ifdef _DEBUG
	std::string window_tag = " [SDL2] ";
	switch (api) {
	case RenderingApiBackend::eNone:
		window_tag += "[UNKNOWN?]";
		break;
	case RenderingApiBackend::eVulkan:
		window_tag += "[Vulkan]";
		break;
	case RenderingApiBackend::eDirectX12:
		window_tag += "[DX12]";
		break;
	case RenderingApiBackend::eOpenGLES:
		window_tag += "[OpenGL ES]";
		break;
	case RenderingApiBackend::eOpenGLModern:
		window_tag += "[OpenGL]";
		break;
	}
	std::string window_name = title.has_value() ? title.value() + window_tag : "New Window" + window_tag;
#else
	std::string window_name = title.value_or("New Window");
#endif
	window = SDL_CreateWindow(
		window_name.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		starting_size.x, starting_size.y,
		flags | SDL_WINDOW_VULKAN
	);
	
	SDL_SetWindowData(window, "SDL2Window", this);
	
	bool is_open_gl = false;
	
	switch (api) {
	case RenderingApiBackend::eOpenGLES:
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		is_open_gl = true;
		break;
	case RenderingApiBackend::eOpenGLModern:
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		is_open_gl = true;
		break;
	default:
		break;
	}
	
	if (is_open_gl) {
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		gl_context = SDL_GL_CreateContext(window);
		if (!gl_context)
			throw std::runtime_error("Failed to create OpenGL context");
	}
}

void SDL2Window::createSurface(bool create_depth_buffer, Optional<gfx::Format> target_color_format,
	Optional<gfx::ColorSpace> target_color_space, Optional<gfx::PresentMethod> target_present_mode,
	Optional<gfx::Format> target_depth_format) {
	GraphicsBackend *driver = GraphicsDriver::get();

	const gfx::Format depth_image_format = target_depth_format.value_or(gfx::Format::eDepth32SfloatStencil8Uint);
	surface_ = driver->surface_create(this, SurfaceDescriptor{
		.label = "SDL2 Window Surface",
		.format = target_color_format,
		.usage = gfx::ImageUsage::eColorAttachment,
		.present_method = target_present_mode,
		.color_space = target_color_space
	});

	if (create_depth_buffer) {
		VkExtent2D window_extent{
			.width = static_cast<uint32_t>(size().x),
			.height = static_cast<uint32_t>(size().y)
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

RID SDL2Window::surface() const {
	return surface_;
}

RID SDL2Window::depthImage() const {
	return depth_image;
}

RID SDL2Window::depthImageView() const {
	return depth_image_view;
}

SharedPtr<IRenderer> SDL2Window::renderer() const { return renderer_; }
void SDL2Window::setRenderer(const SharedPtr<IRenderer> &renderer) { renderer_ = renderer; } 
const SharedPtr<SceneTree> & SDL2Window::sceneTree() const {return scene_tree_; }
void SDL2Window::setSceneTree(const SharedPtr<SceneTree> &scene_tree) { scene_tree_ = scene_tree; }

ivec2 SDL2Window::size() const {
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	return { w, h };
}

void SDL2Window::setSize(const ivec2 &size) const {
	SDL_SetWindowSize(window, size.x, size.y);
}

bool SDL2Window::visible() const {
	const Uint32 flags = SDL_GetWindowFlags(window);
	return (flags & SDL_WINDOW_SHOWN) != 0;
}

void SDL2Window::setVisible(const bool visible) const {
	if (visible) SDL_ShowWindow(window); else SDL_HideWindow(window);
}

void SDL2Window::addSizeChangedCallback(const WindowSizeChangedCallback callback) {
	size_changed_callbacks.push_back(callback);
}

void SDL2Window::addCursorPositionCallback(const WindowCursorPositionCallback callback) {
	cursor_position_callbacks.push_back(callback);
}

void SDL2Window::addKeyCallback(const WindowKeyCallback callback) {
	key_callbacks.push_back(callback);
}

constexpr SDL_Scancode convert(const KeyCode key) {
	switch (key) {
	case eSpace: return SDL_SCANCODE_SPACE;
	case eApostrophe: return SDL_SCANCODE_APOSTROPHE;
	case eComma: return SDL_SCANCODE_COMMA;
	case eMinus: return SDL_SCANCODE_MINUS;
	case ePeriod: return SDL_SCANCODE_PERIOD;
	case eSlash: return SDL_SCANCODE_SLASH;
	case e0: return SDL_SCANCODE_0;
	case e1: return SDL_SCANCODE_1;
	case e2: return SDL_SCANCODE_2;
	case e3: return SDL_SCANCODE_3;
	case e4: return SDL_SCANCODE_4;
	case e5: return SDL_SCANCODE_5;
	case e6: return SDL_SCANCODE_6;
	case e7: return SDL_SCANCODE_7;
	case e8: return SDL_SCANCODE_8;
	case e9: return SDL_SCANCODE_9;
	case eSemicolon: return SDL_SCANCODE_SEMICOLON;
	case eEqual: return SDL_SCANCODE_EQUALS;
	case eA: return SDL_SCANCODE_A;
	case eB: return SDL_SCANCODE_B;
	case eC: return SDL_SCANCODE_C;
	case eD: return SDL_SCANCODE_D;
	case eE: return SDL_SCANCODE_E;
	case eF: return SDL_SCANCODE_F;
	case eG: return SDL_SCANCODE_G;
	case eH: return SDL_SCANCODE_H;
	case eI: return SDL_SCANCODE_I;
	case eJ: return SDL_SCANCODE_J;
	case eK: return SDL_SCANCODE_K;
	case eL: return SDL_SCANCODE_L;
	case eM: return SDL_SCANCODE_M;
	case eN: return SDL_SCANCODE_N;
	case eO: return SDL_SCANCODE_O;
	case eP: return SDL_SCANCODE_P;
	case eQ: return SDL_SCANCODE_Q;
	case eR: return SDL_SCANCODE_R;
	case eS: return SDL_SCANCODE_S;
	case eT: return SDL_SCANCODE_T;
	case eU: return SDL_SCANCODE_U;
	case eV: return SDL_SCANCODE_V;
	case eW: return SDL_SCANCODE_W;
	case eX: return SDL_SCANCODE_X;
	case eY: return SDL_SCANCODE_Y;
	case eZ: return SDL_SCANCODE_Z;
	case eLeftBracket: return SDL_SCANCODE_LEFTBRACKET;
	case eBackslash: return SDL_SCANCODE_BACKSLASH;
	case eRightBracket: return SDL_SCANCODE_RIGHTBRACKET;
	case eGrave: return SDL_SCANCODE_GRAVE;
	case eEscape: return SDL_SCANCODE_ESCAPE;
	case eEnter: return SDL_SCANCODE_RETURN;
	case eTab: return SDL_SCANCODE_TAB;
	case eBackspace: return SDL_SCANCODE_BACKSPACE;
	case eInsert: return SDL_SCANCODE_INSERT;
	case eDelete: return SDL_SCANCODE_DELETE;
	case eRight: return SDL_SCANCODE_RIGHT;
	case eLeft: return SDL_SCANCODE_LEFT;
	case eDown: return SDL_SCANCODE_DOWN;
	case eUp: return SDL_SCANCODE_UP;
	case ePageUp: return SDL_SCANCODE_PAGEUP;
	case ePageDown: return SDL_SCANCODE_PAGEDOWN;
	case eHome: return SDL_SCANCODE_HOME;
	case eEnd: return SDL_SCANCODE_END;
	case eCapsLock: return SDL_SCANCODE_CAPSLOCK;
	case eScrollLock: return SDL_SCANCODE_SCROLLLOCK;
	case eNumLock: return SDL_SCANCODE_NUMLOCKCLEAR;
	case ePrintScreen: return SDL_SCANCODE_PRINTSCREEN;
	case ePause: return SDL_SCANCODE_PAUSE;
	case eF1: return SDL_SCANCODE_F1;
	case eF2: return SDL_SCANCODE_F2;
	case eF3: return SDL_SCANCODE_F3;
	case eF4: return SDL_SCANCODE_F4;
	case eF5: return SDL_SCANCODE_F5;
	case eF6: return SDL_SCANCODE_F6;
	case eF7: return SDL_SCANCODE_F7;
	case eF8: return SDL_SCANCODE_F8;
	case eF9: return SDL_SCANCODE_F9;
	case eF10: return SDL_SCANCODE_F10;
	case eF11: return SDL_SCANCODE_F11;
	case eF12: return SDL_SCANCODE_F12;
	case eF13: return SDL_SCANCODE_F13;
	case eF14: return SDL_SCANCODE_F14;
	case eF15: return SDL_SCANCODE_F15;
	case eF16: return SDL_SCANCODE_F16;
	case eF17: return SDL_SCANCODE_F17;
	case eF18: return SDL_SCANCODE_F18;
	case eF19: return SDL_SCANCODE_F19;
	case eF20: return SDL_SCANCODE_F20;
	case eF21: return SDL_SCANCODE_F21;
	case eF22: return SDL_SCANCODE_F22;
	case eF23: return SDL_SCANCODE_F23;
	case eF24: return SDL_SCANCODE_F24;
	case eKp0: return SDL_SCANCODE_KP_0;
	case eKp1: return SDL_SCANCODE_KP_1;
	case eKp2: return SDL_SCANCODE_KP_2;
	case eKp3: return SDL_SCANCODE_KP_3;
	case eKp4: return SDL_SCANCODE_KP_4;
	case eKp5: return SDL_SCANCODE_KP_5;
	case eKp6: return SDL_SCANCODE_KP_6;
	case eKp7: return SDL_SCANCODE_KP_7;
	case eKp8: return SDL_SCANCODE_KP_8;
	case eKp9: return SDL_SCANCODE_KP_9;
	case eKpDecimal: return SDL_SCANCODE_KP_DECIMAL;
	case eKpDivide: return SDL_SCANCODE_KP_DIVIDE;
	case eKpMultiply: return SDL_SCANCODE_KP_MULTIPLY;
	case eKpSubtract: return SDL_SCANCODE_KP_MINUS;
	case eKpAdd: return SDL_SCANCODE_KP_PLUS;
	case eKpEnter: return SDL_SCANCODE_KP_ENTER;
	case eKpEqual: return SDL_SCANCODE_KP_EQUALS;
	case eLeftShift: return SDL_SCANCODE_LSHIFT;
	case eLeftAlt: return SDL_SCANCODE_LALT;
	case eLeftControl: return SDL_SCANCODE_LCTRL;
	case eRightShift: return SDL_SCANCODE_RSHIFT;
	case eRightAlt: return SDL_SCANCODE_RALT;
	case eRightControl: return SDL_SCANCODE_RCTRL;
	case eMenu: return SDL_SCANCODE_MENU;
	default:
		return static_cast<SDL_Scancode>(key);
	}
}
constexpr KeyCode convert(const SDL_Scancode key) {
	switch (key) {
	case SDL_SCANCODE_SPACE: return eSpace;
	case SDL_SCANCODE_APOSTROPHE: return eApostrophe;
	case SDL_SCANCODE_COMMA: return eComma;
	case SDL_SCANCODE_MINUS: return eMinus;
	case SDL_SCANCODE_PERIOD: return ePeriod;
	case SDL_SCANCODE_SLASH: return eSlash;
	case SDL_SCANCODE_0: return e0;
	case SDL_SCANCODE_1: return e1;
	case SDL_SCANCODE_2: return e2;
	case SDL_SCANCODE_3: return e3;
	case SDL_SCANCODE_4: return e4;
	case SDL_SCANCODE_5: return e5;
	case SDL_SCANCODE_6: return e6;
	case SDL_SCANCODE_7: return e7;
	case SDL_SCANCODE_8: return e8;
	case SDL_SCANCODE_9: return e9;
	case SDL_SCANCODE_SEMICOLON: return eSemicolon;
	case SDL_SCANCODE_EQUALS: return eEqual;
	case SDL_SCANCODE_A: return eA;
	case SDL_SCANCODE_B: return eB;
	case SDL_SCANCODE_C: return eC;
	case SDL_SCANCODE_D: return eD;
	case SDL_SCANCODE_E: return eE;
	case SDL_SCANCODE_F: return eF;
	case SDL_SCANCODE_G: return eG;
	case SDL_SCANCODE_H: return eH;
	case SDL_SCANCODE_I: return eI;
	case SDL_SCANCODE_J: return eJ;
	case SDL_SCANCODE_K: return eK;
	case SDL_SCANCODE_L: return eL;
	case SDL_SCANCODE_M: return eM;
	case SDL_SCANCODE_N: return eN;
	case SDL_SCANCODE_O: return eO;
	case SDL_SCANCODE_P: return eP;
	case SDL_SCANCODE_Q: return eQ;
	case SDL_SCANCODE_R: return eR;
	case SDL_SCANCODE_S: return eS;
	case SDL_SCANCODE_T: return eT;
	case SDL_SCANCODE_U: return eU;
	case SDL_SCANCODE_V: return eV;
	case SDL_SCANCODE_W: return eW;
	case SDL_SCANCODE_X: return eX;
	case SDL_SCANCODE_Y: return eY;
	case SDL_SCANCODE_Z: return eZ;
	case SDL_SCANCODE_LEFTBRACKET: return eLeftBracket;
	case SDL_SCANCODE_BACKSLASH: return eBackslash;
	case SDL_SCANCODE_RIGHTBRACKET: return eRightBracket;
	case SDL_SCANCODE_GRAVE: return eGrave;
	case SDL_SCANCODE_ESCAPE: return eEscape;
	case SDL_SCANCODE_RETURN: return eEnter;
	case SDL_SCANCODE_TAB: return eTab;
	case SDL_SCANCODE_BACKSPACE: return eBackspace;
	case SDL_SCANCODE_INSERT: return eInsert;
	case SDL_SCANCODE_DELETE: return eDelete;
	case SDL_SCANCODE_RIGHT: return eRight;
	case SDL_SCANCODE_LEFT: return eLeft;
	case SDL_SCANCODE_DOWN: return eDown;
	case SDL_SCANCODE_UP: return eUp;
	case SDL_SCANCODE_PAGEUP: return ePageUp;
	case SDL_SCANCODE_PAGEDOWN: return ePageDown;
	case SDL_SCANCODE_HOME: return eHome;
	case SDL_SCANCODE_END: return eEnd;
	case SDL_SCANCODE_CAPSLOCK: return eCapsLock;
	case SDL_SCANCODE_SCROLLLOCK: return eScrollLock;
	case SDL_SCANCODE_NUMLOCKCLEAR: return eNumLock;
	case SDL_SCANCODE_PRINTSCREEN: return ePrintScreen;
	case SDL_SCANCODE_PAUSE: return ePause;
	case SDL_SCANCODE_F1: return eF1;
	case SDL_SCANCODE_F2: return eF2;
	case SDL_SCANCODE_F3: return eF3;
	case SDL_SCANCODE_F4: return eF4;
	case SDL_SCANCODE_F5: return eF5;
	case SDL_SCANCODE_F6: return eF6;
	case SDL_SCANCODE_F7: return eF7;
	case SDL_SCANCODE_F8: return eF8;
	case SDL_SCANCODE_F9: return eF9;
	case SDL_SCANCODE_F10: return eF10;
	case SDL_SCANCODE_F11: return eF11;
	case SDL_SCANCODE_F12: return eF12;
	case SDL_SCANCODE_F13: return eF13;
	case SDL_SCANCODE_F14: return eF14;
	case SDL_SCANCODE_F15: return eF15;
	case SDL_SCANCODE_F16: return eF16;
	case SDL_SCANCODE_F17: return eF17;
	case SDL_SCANCODE_F18: return eF18;
	case SDL_SCANCODE_F19: return eF19;
	case SDL_SCANCODE_F20: return eF20;
	case SDL_SCANCODE_F21: return eF21;
	case SDL_SCANCODE_F22: return eF22;
	case SDL_SCANCODE_F23: return eF23;
	case SDL_SCANCODE_F24: return eF24;
	case SDL_SCANCODE_KP_0: return eKp0;
	case SDL_SCANCODE_KP_1: return eKp1;
	case SDL_SCANCODE_KP_2: return eKp2;
	case SDL_SCANCODE_KP_3: return eKp3;
	case SDL_SCANCODE_KP_4: return eKp4;
	case SDL_SCANCODE_KP_5: return eKp5;
	case SDL_SCANCODE_KP_6: return eKp6;
	case SDL_SCANCODE_KP_7: return eKp7;
	case SDL_SCANCODE_KP_8: return eKp8;
	case SDL_SCANCODE_KP_9: return eKp9;
	case SDL_SCANCODE_KP_DECIMAL: return eKpDecimal;
	case SDL_SCANCODE_KP_DIVIDE: return eKpDivide;
	case SDL_SCANCODE_KP_MULTIPLY: return eKpMultiply;
	case SDL_SCANCODE_KP_MINUS: return eKpSubtract;
	case SDL_SCANCODE_KP_PLUS: return eKpAdd;
	case SDL_SCANCODE_KP_ENTER: return eKpEnter;
	case SDL_SCANCODE_KP_EQUALS: return eKpEqual;
	case SDL_SCANCODE_LSHIFT: return eLeftShift;
	case SDL_SCANCODE_LALT: return eLeftAlt;
	case SDL_SCANCODE_LCTRL: return eLeftControl;
	case SDL_SCANCODE_RSHIFT: return eRightShift;
	case SDL_SCANCODE_RALT: return eRightAlt;
	case SDL_SCANCODE_RCTRL: return eRightControl;
	case SDL_SCANCODE_MENU: return eMenu;
	default:
		return static_cast<KeyCode>(key);
	}
}

bool SDL2Window::pressed(const KeyCode key) const {
	const Uint8 *state = SDL_GetKeyboardState(nullptr);
	return state[convert(key)] != 0;
}

bool SDL2Window::released(const KeyCode key) const {
	const Uint8 *state = SDL_GetKeyboardState(nullptr);
	return state[convert(key)] == 0;
}

bool SDL2Window::justPressed(const KeyCode key) {
	return just_pressed_[key];
}

bool SDL2Window::justReleased(const KeyCode key) {
	return just_released_[key];
}

void SDL2Window::setMouseCaptureMode(const MouseCapture mode) {
	SDL_SetWindowGrab(window, mode == MouseCapture::eNone ? SDL_FALSE : SDL_TRUE);
	SDL_SetWindowMouseGrab(window, mode == MouseCapture::eNone ? SDL_FALSE : SDL_TRUE);
	SDL_SetRelativeMouseMode(mode == MouseCapture::eNone ? SDL_FALSE : SDL_TRUE);
}

vec2 SDL2Window::cursorPosition() const {
	int x, y;
	SDL_GetMouseState(&x, &y);
	return { static_cast<float>(x), static_cast<float>(y) };
}

vec2 SDL2Window::lastCursorPosition() const {
	return last_mouse_coord_;
}

vec2 SDL2Window::mouseDelta() const {
	return mouse_delta_;
}

void SDL2Window::setShouldClose(const bool should) {
	close_requested = should;
}

bool SDL2Window::shouldClose() const {
	return close_requested;
}

void SDL2Window::requiredInstanceExtensions(Vec<const char *> &extensions) const {
	u32 extension_count = 0;
	assert(SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr));
	extensions.resize(extension_count);
	assert(SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data()));
}

HWND SDL2Window::windowHandle() const {
	return 0;
}

void SDL2Window::makeContextCurrent() const {
	SDL_GL_MakeCurrent(window, SDL_GL_GetCurrentContext());
}

void SDL2Window::swapBuffers() const {
	SDL_GL_SwapWindow(window);
}

void SDL2Window::pollEvents() {
	// Reset all just pressed & just released
	for (bool& state : just_pressed_ | std::ranges::views::values)
		state = false;
	for (bool& state : just_released_ | std::ranges::views::values)
		state = false;

	int relative_x, relative_y;
	SDL_GetRelativeMouseState(&relative_x, &relative_y);
	mouse_delta_ = vec2(static_cast<f32>(relative_x), static_cast<f32>(relative_y));
	
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: {
			close_requested = true;
			break;
		}
		case SDL_WINDOWEVENT: {
			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				for (const WindowSizeChangedCallback &callback : size_changed_callbacks) {
					(*callback)(this, { event.window.data1, event.window.data2 });
				}
			}
			break;
		}
		case SDL_MOUSEMOTION: {
			sceneTree()->sendMouseEvent( {
				.delta = vec2(event.motion.x, event.motion.y),
				.delta_relative = vec2(
					static_cast<f32>(event.motion.xrel),
					static_cast<f32>(event.motion.yrel)
				)
			});
			for (const WindowCursorPositionCallback &callback : cursor_position_callbacks) {
				(*callback)(this, { event.motion.x, event.motion.y });
			}
			break;
		}
		case SDL_KEYDOWN: {
			if (event.key.repeat == 0) {
				just_pressed_[convert(event.key.keysym.scancode)] = true;
			}
			
			for (const WindowKeyCallback &callback : key_callbacks) {
				(*callback)(this, convert(event.key.keysym.scancode), event.key.state == SDL_PRESSED ? InputAction::ePress : InputAction::eRelease, InputModifier::eShift);
			}
			break;
		}
		case SDL_KEYUP: {
			if (event.key.repeat == 0) {
				just_released_[convert(event.key.keysym.scancode)] = true;
			}
			for (const WindowKeyCallback &callback : key_callbacks) {
				(*callback)(this, convert(event.key.keysym.scancode), event.key.state == SDL_PRESSED ? InputAction::ePress : InputAction::eRelease, InputModifier::eShift);
			}
			break;
		}
		default:
			break;
		}
	}
}

SDL_Window * SDL2Window::sdl2Window() const {
	return window;
}

WindowDriver SDL2Window::driver() const {
	return WindowDriver::eSdl2;
}
#endif

#ifdef WINDOW_DRIVER_GLFW3
#include "GLFW/glfw3native.h"

GLFW3Window::GLFW3Window() {
}

GLFW3Window::~GLFW3Window() {
	if (!GLFW3Window::disposed()) GLFW3Window::dispose();
}

void GLFW3Window::dispose() {
	GraphicsBackend* driver = GraphicsDriver::get();
	driver->image_view_delete(depth_image_view);
	driver->image_delete(depth_image);
	driver->surface_delete(surface_);
	glfwDestroyWindow(window);
	window = nullptr;
}

bool GLFW3Window::disposed() const {
	return window == nullptr;
}

void GLFW3Window::create(const RenderingApiBackend api, const ivec2 &starting_size, const std::optional<std::string> &title,
                         const std::optional<IWindow *> &shared, const std::optional<WindowConfig> &config) {
	
	switch (api) {
	case RenderingApiBackend::eVulkan:
	case RenderingApiBackend::eDirectX12:
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		break;
	case RenderingApiBackend::eOpenGLES:
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		break;
	case RenderingApiBackend::eOpenGLModern:
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		break;
	case RenderingApiBackend::eNone:
		assert(false && "Unsupported rendering API backend");
		break;
	}

	window = glfwCreateWindow(
		starting_size.x, starting_size.y,
		title.has_value() ? title.value().c_str() : "New Window",
		nullptr, nullptr
	);
	assert(window);
	
	glfwSetErrorCallback([](const int error_code, const char *description) {
		printf("GLFW Error [%d]: %s\n", error_code, description);
	});
	
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
		GLFW3Window *self = static_cast<GLFW3Window *>(glfwGetWindowUserPointer(window));
		for (const WindowSizeChangedCallback &callback : self->size_changed_callbacks)
			(*callback)(self, { width, height });
	});
	
	glfwSetCursorPosCallback(window, [](GLFWwindow *window, const double xpos, const double ypos) {
		GLFW3Window *self = static_cast<GLFW3Window *>(glfwGetWindowUserPointer(window));
		
		vec2 coord(static_cast<f32>(xpos), static_cast<f32>(ypos));
		ivec2 integer_coord(coord);
		
		self->sceneTree()->sendMouseEvent( {
			.position = ivec2(coord),
			.delta_relative = self->lastCursorPosition() - coord
		});
		for (const WindowCursorPositionCallback &callback : self->cursor_position_callbacks)
			(*callback)(self, integer_coord);
		
		self->last_mouse_coord_ = coord;
	});
	
	glfwSetCharCallback(window, [](GLFWwindow *window, const unsigned int codepoint) {
		GLFW3Window *self = static_cast<GLFW3Window *>(glfwGetWindowUserPointer(window));
		for (const WindowCharacterInputCallback &callback : self->character_input_callbacks)
			(*callback)(self, codepoint);
	});
	
	glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, const int action, int mods) {
		GLFW3Window *self = static_cast<GLFW3Window *>(glfwGetWindowUserPointer(window));
		
		switch (action) {
		case GLFW_PRESS:
			self->just_pressed_[static_cast<KeyCode>(key)] = true;
			break;
		case GLFW_RELEASE:
			self->just_released_[static_cast<KeyCode>(key)] = true;
			break;
		}

		for (const WindowKeyCallback &callback : self->key_callbacks) {
			(*callback)(self, static_cast<KeyCode>(key), static_cast<InputAction>(action), static_cast<InputModifier>(mods));
		}
	});
}

void GLFW3Window::createSurface(bool create_depth_buffer, Optional<gfx::Format> target_color_format,
                                Optional<gfx::ColorSpace> target_color_space, Optional<gfx::PresentMethod> target_present_mode,
                                Optional<gfx::Format> target_depth_format) {
	GraphicsBackend *driver = GraphicsDriver::get();

	const gfx::Format depth_image_format = target_depth_format.value_or(gfx::Format::eDepth32SfloatStencil8Uint);
	surface_ = driver->surface_create(this, SurfaceDescriptor{
		                                  .label = "GLFW3 Surface",
		                                  .format = target_color_format,
		                                  .usage = gfx::ImageUsage::eColorAttachment,
		                                  .present_method = target_present_mode,
		                                  .color_space = target_color_space
	                                  });

	if (create_depth_buffer) {
		VkExtent2D window_extent{
			.width = static_cast<uint32_t>(size().x),
			.height = static_cast<uint32_t>(size().y)
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

RID GLFW3Window::surface() const {
	return surface_;
}

RID GLFW3Window::depthImage() const {
	return depth_image;
}

RID GLFW3Window::depthImageView() const {
	return depth_image_view;
}

SharedPtr<IRenderer> GLFW3Window::renderer() const {
	return renderer_;
}

void GLFW3Window::setRenderer(const SharedPtr<IRenderer> &renderer) {
	renderer_ = renderer;
}

const SharedPtr<SceneTree> & GLFW3Window::sceneTree() const {
	return scene_tree_;
}

void GLFW3Window::setSceneTree(const SharedPtr<SceneTree> &scene_tree) {
	scene_tree_ = scene_tree;
}

ivec2 GLFW3Window::size() const {
	if (!window) return { 1, 1 };
	ivec2 size;
	glfwGetWindowSize(window, &size.x, &size.y);
	return size;
}

void GLFW3Window::setSize(const ivec2 &size) const {
	glfwSetWindowSize(window, size.x, size.y);
}

bool GLFW3Window::visible() const {
	return glfwGetWindowAttrib(window, GLFW_VISIBLE) == GLFW_TRUE;
}
void GLFW3Window::setVisible(const bool visible) const {
	if (visible) glfwShowWindow(window); else glfwHideWindow(window);
}

bool GLFW3Window::pressed(const KeyCode key) const {
	return glfwGetKey(window, key) != GLFW_RELEASE;
}

bool GLFW3Window::released(const KeyCode key) const {
	return glfwGetKey(window, key) == GLFW_RELEASE;
}

bool GLFW3Window::justPressed(const KeyCode key) {
	return just_pressed_[key];
}

bool GLFW3Window::justReleased(const KeyCode key) {
	return just_released_[key];
}

void GLFW3Window::addSizeChangedCallback(const WindowSizeChangedCallback callback) {
	size_changed_callbacks.push_back(callback);
}

void GLFW3Window::addCursorPositionCallback(const WindowCursorPositionCallback callback) {
	cursor_position_callbacks.push_back(callback);
}

void GLFW3Window::addKeyCallback(const WindowKeyCallback callback) {
	key_callbacks.push_back(callback);
}

void GLFW3Window::setMouseCaptureMode(const MouseCapture mode) {
	switch (mode) {
	case MouseCapture::eNone:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case MouseCapture::eCaptured:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	case MouseCapture::Confined:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	}
}
vec2 GLFW3Window::cursorPosition() const {
	f64 x, y;
	glfwGetCursorPos(window, &x, &y);
	return {
		static_cast<f32>(x),
		static_cast<f32>(y)
	};
}
vec2 GLFW3Window::lastCursorPosition() const {
	return last_mouse_coord_;
}

vec2 GLFW3Window::mouseDelta() const {
	return vec2(0.0f); // not doing this right now
}
void GLFW3Window::setShouldClose(const bool should) {
	glfwSetWindowShouldClose(window, should ? GLFW_TRUE : GLFW_FALSE);
}
bool GLFW3Window::shouldClose() const {
	return glfwWindowShouldClose(window) == GLFW_TRUE;
}

void GLFW3Window::requiredInstanceExtensions(Vec<const char *> &extensions) const {
	uint32_t extension_count = 0;
	glfwGetRequiredInstanceExtensions(&extension_count);
	extensions.resize(extension_count);
	const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extension_count);
	for (u32 i = 0; i < extension_count; ++i) {
		extensions[i] = glfw_extensions[i];
	}
}

HWND GLFW3Window::windowHandle() const {
	return glfwGetWin32Window(window);
}

void GLFW3Window::makeContextCurrent() const {
	glfwMakeContextCurrent(window);
}

void GLFW3Window::swapBuffers() const {
	glfwSwapBuffers(window);
}

void GLFW3Window::pollEvents() {
	for (bool& state : just_pressed_ | std::ranges::views::values)
		state = false;
	for (bool& state : just_released_ | std::ranges::views::values)
		state = false;
	
	glfwPollEvents();
}

WindowDriver GLFW3Window::driver() const {
	return WindowDriver::eGlfw3;
}

GLFWwindow * GLFW3Window::glfw3Window() const {
	return window;
}

#endif

Window::Window(const WindowDriver driver) {
	switch (driver) {
#ifdef WINDOW_DRIVER_SDL2
	case WindowDriver::eSdl2:
		window_impl = std::make_unique<SDL2Window>();
		break;
#endif
#ifdef WINDOW_DRIVER_GLFW3
	case WindowDriver::eGlfw3:
		window_impl = std::make_unique<GLFW3Window>();
		break;
#endif
	}
}
Window::Window(const WindowDriver driver, const RenderingApiBackend api, ivec2 const &starting_size,
               std::optional<std::string> const &title, std::optional<IWindow *> const &shared,
               std::optional<WindowConfig> const &config) : Window(driver) {
	Window::create(api, starting_size, title, shared, config);
}
Window::~Window() {
	if (!Window::disposed())
		Window::dispose();
}

void Window::create(const RenderingApiBackend api, ivec2 const &starting_size, std::optional<std::string> const &title,
                    std::optional<IWindow *> const &shared, std::optional<WindowConfig> const &config) {
	window_impl->create(api, starting_size, title, shared, config);
}

void Window::createSurface(const bool create_depth_buffer, const Optional<gfx::Format> target_color_format,
                           const Optional<gfx::ColorSpace> target_color_space, const Optional<gfx::PresentMethod> target_present_mode,
                           const Optional<gfx::Format> target_depth_format) {
	window_impl->createSurface(create_depth_buffer, target_color_format, target_color_space, target_present_mode, target_depth_format);
}

RID Window::surface() const {
	return window_impl->surface();
}

RID Window::depthImage() const {
	return window_impl->depthImage();
}

RID Window::depthImageView() const {
	return window_impl->depthImageView();
}

void Window::dispose() {
	window_impl->dispose();
}
bool Window::disposed() const {
	return window_impl->disposed();
}
void Window::setRenderer(SharedPtr<IRenderer> const &renderer) {
	window_impl->setRenderer(renderer);
}
SharedPtr<IRenderer> Window::renderer() const {
	return window_impl->renderer();
}
void Window::setSceneTree(SharedPtr<SceneTree> const &scene_tree) {
	window_impl->setSceneTree(scene_tree);
}
SharedPtr<SceneTree> const & Window::sceneTree() const {
	return window_impl->sceneTree();
}
ivec2 Window::size() const {
	return window_impl->size();
}
void Window::setSize(ivec2 const &size) const {
	window_impl->setSize(size);
}
void Window::setVisible(bool const visible) const {
	window_impl->setVisible(visible);
}
bool Window::visible() const {
	return window_impl->visible();
}
ivec4 Window::viewport() const {
	return { 0, 0, size().x, size().y };
}

void Window::setShouldClose(const bool should) {
	window_impl->setShouldClose(should);
}

bool Window::shouldClose() const {
	return window_impl->shouldClose();
}

void Window::addSizeChangedCallback(const WindowSizeChangedCallback callback) {
	window_impl->addSizeChangedCallback(callback);
}

void Window::addCursorPositionCallback(const WindowCursorPositionCallback callback) {
	window_impl->addCursorPositionCallback(callback);
}

void Window::addKeyCallback(const WindowKeyCallback callback) {
	window_impl->addKeyCallback(callback);
}

bool Window::pressed(const KeyCode key) const {
	return window_impl->pressed(key);
}

bool Window::released(const KeyCode key) const {
	return window_impl->released(key);
}

bool Window::justPressed(const KeyCode key) {
	return window_impl->justPressed(key);
}

bool Window::justReleased(const KeyCode key) {
	return window_impl->justReleased(key);
}

void Window::setMouseCaptureMode(const MouseCapture mode) {
	window_impl->setMouseCaptureMode(mode);
}

vec2 Window::cursorPosition() const {
	return window_impl->cursorPosition();
}

vec2 Window::lastCursorPosition() const {
	return window_impl->lastCursorPosition();
}

vec2 Window::mouseDelta() const {
	return window_impl->mouseDelta();
}

void Window::requiredInstanceExtensions(Vec<const char *> &extensions) const {
	window_impl->requiredInstanceExtensions(extensions);
}

void Window::swapBuffers() const {
	window_impl->swapBuffers();
}

void Window::pollEvents() {
	window_impl->pollEvents();
}

void Window::makeContextCurrent() const {
	window_impl->makeContextCurrent();
}

WindowDriver Window::driver() const {
	return window_impl->driver();
}

HWND Window::windowHandle() const {
	return window_impl->windowHandle();
}
