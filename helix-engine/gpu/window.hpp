#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vulkan/vulkan.hpp>

#include "driver.hpp"
#include "math.hpp"
#include "types.hpp"
#include "engine/disposable.hpp"
#include "engine/rid.hpp"

#define WINDOW_DRIVER_SDL2
#define WINDOW_DRIVER_GLFW3

#ifdef WINDOW_DRIVER_GLFW3
#include "glfw/glfw3.h"
#endif

#ifdef WINDOW_DRIVER_SDL2
#include <SDL2/SDL.h>
#endif

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

class IWindow;

enum class MouseCapture {
	eNone,
	eCaptured,
	Confined
};

enum KeyCode : u16 {
	eSpace = 32,
	eApostrophe = 39, /* ' */
	eComma = 44, /* , */
	eMinus = 45, /* - */
	ePeriod = 46, /* . */
	eSlash = 47, /* / */
	e0 = 48,
	e1 = 49,
	e2 = 50,
	e3 = 51,
	e4 = 52,
	e5 = 53,
	e6 = 54,	
	e7 = 55,
	e8 = 56,
	e9 = 57,
	eSemicolon = 59, /* ; */
	eEqual = 61, /* = */
	eA = 65,
	eB = 66,
	eC = 67,
	eD = 68,
	eE = 69,
	eF = 70,
	eG = 71,
	eH = 72,
	eI = 73,
	eJ = 74,
	eK = 75,
	eL = 76,
	eM = 77,
	eN = 78,
	eO = 79,
	eP = 80,
	eQ = 81,
	eR = 82,
	eS = 83,
	eT = 84,
	eU = 85,
	eV = 86,
	eW = 87,
	eX = 88,
	eY = 89,
	eZ = 90,
	eLeftBracket = 91,  /* [ */
	eBackslash = 92,     /* \ */
	eRightBracket = 93, /* ] */
	eGrave = 96,  /* ` */
	eWorld1 = 161,      /* non-US #1 */
	eWorld2 = 162,      /* non-US #2 */
	eEscape = 256,
	eEnter = 257,
	eTab = 258,
	eBackspace = 259,
	eInsert = 260,
	eDelete = 261,
	eRight = 262,
	eLeft = 263,
	eDown = 264,
	eUp = 265,
	ePageUp = 266,
	ePageDown = 267,
	eHome = 268,
	eEnd = 269,
	eCapsLock = 280,
	eScrollLock = 281,
	eNumLock = 282,
	ePrintScreen = 283,
	ePause = 284,
	eF1 = 290,
	eF2 = 291,
	eF3 = 292,
	eF4 = 293,
	eF5 = 294,
	eF6 = 295,
	eF7 = 296,
	eF8 = 297,
	eF9 = 298,
	eF10 = 299,
	eF11 = 300,
	eF12 = 301,
	eF13 = 302,
	eF14 = 303,
	eF15 = 304,
	eF16 = 305,
	eF17 = 306,
	eF18 = 307,
	eF19 = 308,
	eF20 = 309,
	eF21 = 310,
	eF22 = 311,
	eF23 = 312,
	eF24 = 313,
	eF25 = 314,
	eKp0 = 320,
	eKp1 = 321,
	eKp2 = 322,
	eKp3 = 323,
	eKp4 = 324,
	eKp5 = 325,
	eKp6 = 326,
	eKp7 = 327,
	eKp8 = 328,
	eKp9 = 329,
	eKpDecimal = 330,
	eKpDivide = 331,
	eKpMultiply = 332,
	eKpSubtract = 333,
	eKpAdd = 334,
	eKpEnter = 335,
	eKpEqual = 336,
	eLeftShift = 340,
	eLeftControl = 341,
	eLeftAlt = 342,
	eRightShift = 344,
	eRightControl = 345,
	eRightAlt = 346,
	eMenu = 348
};

constexpr char const *toString(KeyCode const e) {
	switch (e) {
		case eSpace: return "KEY_SPACE";
		case eApostrophe: return "KEY_APOSTROPHE";
		case eComma: return "KEY_COMMA";
		case eMinus: return "KEY_MINUS";
		case ePeriod: return "KEY_PERIOD";
		case eSlash: return "KEY_SLASH";
		case e0: return "KEY_0";
		case e1: return "KEY_1";
		case e2: return "KEY_2";
		case e3: return "KEY_3";
		case e4: return "KEY_4";
		case e5: return "KEY_5";
		case e6: return "KEY_6";
		case e7: return "KEY_7";
		case e8: return "KEY_8";
		case e9: return "KEY_9";
		case eSemicolon: return "KEY_SEMICOLON";
		case eEqual: return "KEY_EQUAL";
		case eA: return "KEY_A";
		case eB: return "KEY_B";
		case eC: return "KEY_C";
		case eD: return "KEY_D";
		case eE: return "KEY_E";
		case eF: return "KEY_F";
		case eG: return "KEY_G";
		case eH: return "KEY_H";
		case eI: return "KEY_I";
		case eJ: return "KEY_J";
		case eK: return "KEY_K";
		case eL: return "KEY_L";
		case eM: return "KEY_M";
		case eN: return "KEY_N";
		case eO: return "KEY_O";
		case eP: return "KEY_P";
		case eQ: return "KEY_Q";
		case eR: return "KEY_R";
		case eS: return "KEY_S";
		case eT: return "KEY_T";
		case eU: return "KEY_U";
		case eV: return "KEY_V";
		case eW: return "KEY_W";
		case eX: return "KEY_X";
		case eY: return "KEY_Y";
		case eZ: return "KEY_Z";
		case eLeftBracket: return "KEY_LEFT_BRACKET";
		case eBackslash: return "KEY_BACKSLASH";
		case eRightBracket: return "KEY_RIGHT_BRACKET";
		case eGrave: return "KEY_GRAVE_ACCENT";
		case eWorld1: return "KEY_WORLD_1";
		case eWorld2: return "KEY_WORLD_2";
		case eEscape: return "KEY_ESCAPE";
		case eEnter: return "KEY_ENTER";
		case eTab: return "KEY_TAB";
		case eBackspace: return "KEY_BACKSPACE";
		case eInsert: return "KEY_INSERT";
		case eDelete: return "KEY_DELETE";
		case eRight: return "KEY_RIGHT";
		case eLeft: return "KEY_LEFT";
		case eDown: return "KEY_DOWN";
		case eUp: return "KEY_UP";
		case ePageUp: return "KEY_PAGE_UP";
		case ePageDown: return "KEY_PAGE_DOWN";
		case eHome: return "KEY_HOME";
		case eEnd: return "KEY_END";
		case eCapsLock: return "KEY_CAPS_LOCK";
		case eScrollLock: return "KEY_SCROLL_LOCK";
		case eNumLock: return "KEY_NUM_LOCK";
		case ePrintScreen: return "KEY_PRINT_SCREEN";
		case ePause: return "KEY_PAUSE";
		case eF1: return "KEY_F1";
		case eF2: return "KEY_F2";
		case eF3: return "KEY_F3";
		case eF4: return "KEY_F4";
		case eF5: return "KEY_F5";
		case eF6: return "KEY_F6";
		case eF7: return "KEY_F7";
		case eF8: return "KEY_F8";
		case eF9: return "KEY_F9";
		case eF10: return "KEY_F10";
		case eF11: return "KEY_F11";
		case eF12: return "KEY_F12";
		case eF13: return "KEY_F13";
		case eF14: return "KEY_F14";
		case eF15: return "KEY_F15";
		case eF16: return "KEY_F16";
		case eF17: return "KEY_F17";
		case eF18: return "KEY_F18";
		case eF19: return "KEY_F19";
		case eF20: return "KEY_F20";
		case eF21: return "KEY_F21";
		case eF22: return "KEY_F22";
		case eF23: return "KEY_F23";
		case eF24: return "KEY_F24";
		case eF25: return "KEY_F25";
		case eKp0: return "KEY_KP_0";
		case eKp1: return "KEY_KP_1";
		case eKp2: return "KEY_KP_2";
		case eKp3: return "KEY_KP_3";
		case eKp4: return "KEY_KP_4";
		case eKp5: return "KEY_KP_5";
		case eKp6: return "KEY_KP_6";
		case eKp7: return "KEY_KP_7";
		case eKp8: return "KEY_KP_8";
		case eKp9: return "KEY_KP_9";
		case eKpDecimal: return "KEY_KP_DECIMAL";
		case eKpDivide: return "KEY_KP_DIVIDE";
		case eKpMultiply: return "KEY_KP_MULTIPLY";
		case eKpSubtract: return "KEY_KP_SUBTRACT";
		case eKpAdd: return "KEY_KP_ADD";
		case eKpEnter: return "KEY_KP_ENTER";
		case eKpEqual: return "KEY_KP_EQUAL";
		case eLeftShift: return "KEY_LEFT_SHIFT";
		case eLeftControl: return "KEY_LEFT_CONTROL";
		case eLeftAlt: return "KEY_LEFT_ALT";
		case eRightShift: return "KEY_RIGHT_SHIFT";
		case eRightControl: return "KEY_RIGHT_CONTROL";
		case eRightAlt: return "KEY_RIGHT_ALT";
		case eMenu: return "KEY_MENU";
	}
	return "KEY_UNKNOWN";
}

enum class InputModifier : u8 {
	eShift = 0x0001,
	eControl = 0x0002,
	eAlt = 0x0004,
	eSuper = 0x0008,
	eCapsLock = 0x0010,
	eNumLock = 0x0020
};
enum class InputAction : u8 {
	ePress = 0x01,
	eRelease = 0x02,
	eRepeat = 0x03
};

enum class WindowDriver {
	eNone,
	eGlfw3,
	eSdl2
};

struct MouseInputEvent {
	ivec2 position;
	ivec2 delta;
	vec2 position_relative;
	vec2 delta_relative;
};

using WindowSizeChangedCallback = void(*)(IWindow *, ivec2);
using WindowCursorPositionCallback = void(*)(IWindow *, vec2);
using WindowCharacterInputCallback = void(*)(IWindow *, u32);
using WindowKeyCallback = void(*)(IWindow *, KeyCode, InputAction, InputModifier);

class IWindow : public IDisposable {
public:
	~IWindow() override = default;
	
	virtual void create(RenderingApiBackend api, ivec2 const &starting_size, _STD optional<_STD string> const &title = _STD nullopt,
		_STD optional<IWindow*> const &shared = _STD nullopt,
		_STD optional<WindowConfig> const &config = _STD nullopt) = 0;
	
	virtual void createSurface(bool create_depth_buffer, Optional<gfx::Format> target_color_format,
		Optional<gfx::ColorSpace> target_color_space, Optional<gfx::PresentMethod> target_present_mode,
		Optional<gfx::Format> target_depth_format) = 0;
	
	[[nodiscard]] virtual RID surface() const = 0;
	[[nodiscard]] virtual RID depthImage() const = 0;
	[[nodiscard]] virtual RID depthImageView() const = 0;
	
	[[nodiscard]] virtual SharedPtr<IRenderer> renderer() const = 0;
	virtual void setRenderer(SharedPtr<IRenderer> const& renderer) = 0;

	[[nodiscard]] virtual SharedPtr<SceneTree> const& sceneTree() const = 0;
	virtual void setSceneTree(SharedPtr<SceneTree> const& scene_tree) = 0;
	
	[[nodiscard]] virtual ivec2 size() const = 0;
	virtual void setSize(ivec2 const& size) const = 0;
	
	[[nodiscard]] virtual bool visible() const = 0;
	virtual void setVisible(bool visible) const = 0;
	
	[[nodiscard]] virtual bool shouldClose() const = 0;
	virtual void setShouldClose(bool should) = 0;
	
	[[nodiscard]] virtual std::string_view title() const = 0;
	virtual void setTitle(std::string_view title) = 0;
	
	[[nodiscard]] virtual f64 time() const = 0;
	[[nodiscard]] virtual u64 ticks() const = 0;
	
	virtual void addSizeChangedCallback(WindowSizeChangedCallback callback) = 0;
	virtual void addCursorPositionCallback(WindowCursorPositionCallback callback) = 0;
	virtual void addKeyCallback(WindowKeyCallback callback) = 0;
	
	[[nodiscard]] virtual bool pressed(KeyCode key) const = 0;
	[[nodiscard]] virtual bool released(KeyCode key) const = 0;
	[[nodiscard]] virtual bool justPressed(KeyCode key) = 0;
	[[nodiscard]] virtual bool justReleased(KeyCode key) = 0;
	
	[[nodiscard]] f32 axis1(const KeyCode pos, const KeyCode neg) const {
		return (pressed(pos) ? 1.0f : 0.0f) - (pressed(neg) ? 1.0f : 0.0f);
	}
	
	[[nodiscard]] vec2 axis2(const KeyCode pos_x, const KeyCode neg_x, const KeyCode pos_y, const KeyCode neg_y) const {
		return {axis1(pos_x, neg_x), axis1(pos_y, neg_y)};
	}
	
	virtual void setMouseCaptureMode(MouseCapture mode) = 0;
	[[nodiscard]] virtual vec2 cursorPosition() const = 0;
	[[nodiscard]] virtual vec2 lastCursorPosition() const = 0;
	[[nodiscard]] virtual vec2 mouseDelta() const = 0;
	
	virtual void pollEvents() = 0;

	/**
	 * \brief <b>Vulkan Only!</b>
	 * \param extensions A vector to be filled with the required instance extensions.
	 */
	virtual void requiredInstanceExtensions(Vector<const char *> &extensions) const = 0;
	
	[[nodiscard]] virtual WindowDriver driver() const = 0;
	
#ifdef _WIN32
	
	/**
	 * \brief <b>Windows Only!</b>
	 * \return The native window handle (HWND) for the window.
	 */
	[[nodiscard]] virtual HWND windowHandle() const = 0;

#endif
	
	/**
	 * \brief <b>Open GL Only!</b>
	 */
	virtual void makeContextCurrent() const = 0;
	/**
	 * \brief <b>Open GL Only!</b>
	 */
	virtual void swapBuffers() const = 0;
};

#ifdef WINDOW_DRIVER_SDL2

class SDL2Window : public IWindow {
public:
	SDL2Window();
	~SDL2Window() override;
	
	SDL2Window(SDL2Window const& window) = delete;
	SDL2Window(SDL2Window&& window) = delete;
	SDL2Window& operator=(SDL2Window const& window) = delete;
	SDL2Window& operator=(SDL2Window&& window) = delete;
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
	
	void create(RenderingApiBackend api, const ivec2 &starting_size, const std::optional<std::string> &title = std::nullopt,
		const std::optional<IWindow*> &shared = std::nullopt,
		const std::optional<WindowConfig> &config = std::nullopt) override;
	
	void createSurface(bool create_depth_buffer, Optional<gfx::Format> target_color_format,
		Optional<gfx::ColorSpace> target_color_space, Optional<gfx::PresentMethod> target_present_mode,
		Optional<gfx::Format> target_depth_format) override;
	
	[[nodiscard]] RID surface() const override;
	[[nodiscard]] RID depthImage() const override;
	[[nodiscard]] RID depthImageView() const override;
	
	[[nodiscard]] SharedPtr<IRenderer> renderer() const override;
	void setRenderer(const SharedPtr<IRenderer> &renderer) override;
	
	[[nodiscard]] const SharedPtr<SceneTree> & sceneTree() const override;
	void setSceneTree(const SharedPtr<SceneTree> &scene_tree) override;
	
	[[nodiscard]] ivec2 size() const override;
	void setSize(const ivec2 &size) const override;
	
	[[nodiscard]] bool visible() const override;
	void setVisible(bool visible) const override;
	
	[[nodiscard]] std::string_view title() const override;
	void setTitle(std::string_view title) override;
	
	[[nodiscard]] f64 time() const override;
	[[nodiscard]] u64 ticks() const override;
	
	void addSizeChangedCallback(WindowSizeChangedCallback callback) override;
	void addCursorPositionCallback(WindowCursorPositionCallback callback) override;
	void addKeyCallback(WindowKeyCallback callback) override;
	
	[[nodiscard]] bool pressed(KeyCode key) const override;
	[[nodiscard]] bool released(KeyCode key) const override;
	[[nodiscard]] bool justPressed(KeyCode key) override;
	[[nodiscard]] bool justReleased(KeyCode key) override;
	
	void setMouseCaptureMode(MouseCapture mode) override;
	[[nodiscard]] vec2 cursorPosition() const override;
	[[nodiscard]] vec2 lastCursorPosition() const override;
	[[nodiscard]] vec2 mouseDelta() const override;

	void setShouldClose(bool should) override;
	[[nodiscard]] bool shouldClose() const override;
	
	void requiredInstanceExtensions(Vector<const char *> &extensions) const override;

	/**
	 * \brief UNIMPLEMENTED CURRENTLY
	 */
	[[nodiscard]] HWND windowHandle() const override;
	
	void makeContextCurrent() const override;
	void swapBuffers() const override;
	
	void pollEvents() override;

	[[nodiscard]] SDL_Window* sdl2Window() const;
	[[nodiscard]] WindowDriver driver() const override;
	

private:
	SharedPtr<IRenderer> renderer_;
	SharedPtr<SceneTree> scene_tree_;
	SDL_Window *window = nullptr;
	SDL_GLContext gl_context = nullptr;
	
	Vector<WindowSizeChangedCallback> size_changed_callbacks;
	Vector<WindowCursorPositionCallback> cursor_position_callbacks;
	Vector<WindowCharacterInputCallback> character_input_callbacks;
	Vector<WindowKeyCallback> key_callbacks;
	
	RID surface_;
	RID depth_image;
	RID depth_image_view;
	
	bool has_swapchain = false;
	bool has_depth_attachment = false;
	bool has_command_buffers = false;
	bool close_requested = false;
	
	gfx::Format depth_format = gfx::Format::eDepth24UnormStencil8Uint;
	gfx::Format color_format = gfx::Format::eRgba8Srgb;
	gfx::ColorSpace color_space = gfx::ColorSpace::eSrgbNonLinear;
	gfx::PresentMethod present_mode = gfx::PresentMethod::eFifo; // Fifo is available on every single platform.
	Extent2D extent = { .width = 0, .height = 0 };
	
	vec2 last_mouse_coord_;
	vec2 mouse_delta_ = vec2(0.0f);
	UnorderedMap<KeyCode, bool> just_pressed_;
	UnorderedMap<KeyCode, bool> just_released_;
};

#endif

class GLFW3Window : public IWindow {
public:
	GLFW3Window();
	~GLFW3Window() override;
	
	GLFW3Window(GLFW3Window const& window) = delete;
	GLFW3Window(GLFW3Window&& window) = delete;
	GLFW3Window& operator=(GLFW3Window const& window) = delete;
	GLFW3Window& operator=(GLFW3Window&& window) = delete;
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
	
	void create(RenderingApiBackend api, const ivec2 &starting_size, const std::optional<std::string> &title = std::nullopt,
		const std::optional<IWindow*> &shared = std::nullopt,
		const std::optional<WindowConfig> &config = std::nullopt) override;
	
	void createSurface(bool create_depth_buffer, Optional<gfx::Format> target_color_format,
		Optional<gfx::ColorSpace> target_color_space, Optional<gfx::PresentMethod> target_present_mode,
		Optional<gfx::Format> target_depth_format) override;
	
	[[nodiscard]] RID surface() const override;
	[[nodiscard]] RID depthImage() const override;
	[[nodiscard]] RID depthImageView() const override;
	
	[[nodiscard]] SharedPtr<IRenderer> renderer() const override;
	void setRenderer(const SharedPtr<IRenderer> &renderer) override;
	
	[[nodiscard]] const SharedPtr<SceneTree> & sceneTree() const override;
	void setSceneTree(const SharedPtr<SceneTree> &scene_tree) override;
	
	[[nodiscard]] ivec2 size() const override;
	void setSize(const ivec2 &size) const override;
	
	[[nodiscard]] bool visible() const override;
	void setVisible(bool visible) const override;
	
	[[nodiscard]] std::string_view title() const override;
	void setTitle(std::string_view title) override;
	
	[[nodiscard]] f64 time() const override;
	[[nodiscard]] u64 ticks() const override;
	[[nodiscard]] bool pressed(KeyCode key) const override;
	[[nodiscard]] bool released(KeyCode key) const override;
	[[nodiscard]] bool justPressed(KeyCode key) override;
	[[nodiscard]] bool justReleased(KeyCode key) override;
	
	
	void addSizeChangedCallback(WindowSizeChangedCallback callback) override;
	void addCursorPositionCallback(WindowCursorPositionCallback callback) override;
	void addKeyCallback(WindowKeyCallback callback) override;
	
	void setMouseCaptureMode(MouseCapture mode) override;
	[[nodiscard]] vec2 cursorPosition() const override;
	[[nodiscard]] vec2 lastCursorPosition() const override;
	[[nodiscard]] vec2 mouseDelta() const override;
	
	void setShouldClose(bool should) override;
	[[nodiscard]] bool shouldClose() const override;
	
	void requiredInstanceExtensions(Vector<const char *> &extensions) const override;

	/**
	 * \brief UNIMPLEMENTED CURRENTLY
	 */
	[[nodiscard]] HWND windowHandle() const override;
	
	void makeContextCurrent() const override;
	void swapBuffers() const override;
	
	void pollEvents() override;

	[[nodiscard]] WindowDriver driver() const override;
	[[nodiscard]] GLFWwindow* glfw3Window() const;

private:
	SharedPtr<IRenderer> renderer_;
	SharedPtr<SceneTree> scene_tree_;
	GLFWwindow *window = nullptr;
	
	RID surface_;
	RID depth_image;
	RID depth_image_view;
	
	Vector<WindowSizeChangedCallback> size_changed_callbacks;
	Vector<WindowCursorPositionCallback> cursor_position_callbacks;
	Vector<WindowCharacterInputCallback> character_input_callbacks;
	Vector<WindowKeyCallback> key_callbacks;
	
	bool has_swapchain = false;
	bool has_depth_attachment = false;
	bool has_command_buffers = false;
	bool close_requested = false;
	
	gfx::Format depth_format = gfx::Format::eDepth24UnormStencil8Uint;
	gfx::Format color_format = gfx::Format::eRgba8Srgb;
	gfx::ColorSpace color_space = gfx::ColorSpace::eSrgbNonLinear;
	gfx::PresentMethod present_mode = gfx::PresentMethod::eFifo; // Fifo is available on every single platform.
	Extent2D extent = { .width = 0, .height = 0 };
	
	vec2 last_mouse_coord_;
	UnorderedMap<KeyCode, bool> just_pressed_;
	UnorderedMap<KeyCode, bool> just_released_;
};

class Window : public IWindow {
public:
	Window(WindowDriver driver);
	Window(WindowDriver driver, RenderingApiBackend api, ivec2 const &starting_size, _STD optional<_STD string> const &title = _STD nullopt,
		_STD optional<IWindow*> const &shared = _STD nullopt,
		_STD optional<WindowConfig> const &config = _STD nullopt);
	~Window() override;
	
	void create(RenderingApiBackend api, ivec2 const &starting_size, _STD optional<_STD string> const &title = _STD nullopt,
		_STD optional<IWindow*> const &shared = _STD nullopt,
		_STD optional<WindowConfig> const &config = _STD nullopt) override;
	
	void createSurface(bool create_depth_buffer, Optional<gfx::Format> target_color_format, Optional<gfx::ColorSpace> target_color_space,
		Optional<gfx::PresentMethod> target_present_mode, Optional<gfx::Format> target_depth_format) override;
	
	[[nodiscard]] RID surface() const override;
	[[nodiscard]] RID depthImage() const override;
	[[nodiscard]] RID depthImageView() const override;
	
	// no copy no move
	Window(Window const& window) = delete;
	Window(Window&& window) = delete;
	Window& operator=(Window const& window) = delete;
	Window& operator=(Window&& window) = delete;
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

	void setRenderer(SharedPtr<IRenderer> const& renderer) override;
	[[nodiscard]] SharedPtr<IRenderer> renderer() const override;
	
	void setSceneTree(SharedPtr<SceneTree> const& scene_tree) override;
	[[nodiscard]] SharedPtr<SceneTree> const& sceneTree() const override;
	
	[[nodiscard]] ivec2 size() const override;
	void setSize(ivec2 const& size) const override;

	void setVisible(bool visible) const override;
	[[nodiscard]] bool visible() const override;

	[[nodiscard]] std::string_view title() const override;
	void setTitle(std::string_view title) override;
	
	[[nodiscard]] ivec4 viewport() const;

	[[nodiscard]] f64 time() const override;
	[[nodiscard]] u64 ticks() const override;
	
	void setShouldClose(bool should) override;
	[[nodiscard]] bool shouldClose() const override;

	void addSizeChangedCallback(WindowSizeChangedCallback callback) override;
	void addCursorPositionCallback(WindowCursorPositionCallback callback) override;
	void addKeyCallback(WindowKeyCallback callback) override;
	
	[[nodiscard]] bool pressed(KeyCode key) const override;
	[[nodiscard]] bool released(KeyCode key) const override;
	[[nodiscard]] bool justPressed(KeyCode key) override;
	[[nodiscard]] bool justReleased(KeyCode key) override;
	
	void setMouseCaptureMode(MouseCapture mode) override;
	[[nodiscard]] vec2 cursorPosition() const override;
	[[nodiscard]] vec2 lastCursorPosition() const override;
	[[nodiscard]] vec2 mouseDelta() const override;
	
	void requiredInstanceExtensions(Vector<const char *> &extensions) const override;
	
	void swapBuffers() const override;
	void pollEvents() override;
	void makeContextCurrent() const override;
	
	[[nodiscard]] WindowDriver driver() const override;
	[[nodiscard]] HWND windowHandle() const override;

	friend class GraphicsDriver;
private:
	UniquePtr<IWindow> window_impl = nullptr;
};