#pragma once

#include <memory>

#include "types.hpp"
#include "inipp/inipp.h"

class EditorCamera3D;
class Window;
class SceneTree;
class IRenderer;

class IMainLoop {
public:
	virtual ~IMainLoop() = default;

	virtual [[nodiscard]] Result<> start(std::string const &startup_scene) = 0;
	virtual [[nodiscard]] Result<> iter(f64 delta) = 0;
	virtual [[nodiscard]] Result<> stop() = 0;
	virtual [[nodiscard]] Result<bool> running() = 0;
};

class Main final {
public:
	static [[nodiscard]] Result<> start(UniquePtr<IMainLoop> &&main_loop, std::string const &startup_scene);
	static [[nodiscard]] Result<> iter(f64 delta);
	static [[nodiscard]] Result<> stop();
	static [[nodiscard]] Result<bool> running();

	static [[nodiscard]] Result<IMainLoop &> mainLoop();

private:
	static UniquePtr<IMainLoop> main_loop_;
};

class DefMainLoop final : public IMainLoop {
public:
	[[nodiscard]] Result<> start(std::string const &startup_scene) override;
	[[nodiscard]] Result<> iter(f64 delta) override;
	[[nodiscard]] Result<> stop() override;
	[[nodiscard]] Result<bool> running() override;

	[[nodiscard]] SharedPtr<Window> window() const;
	[[nodiscard]] SharedPtr<IRenderer> renderer() const;
	[[nodiscard]] SharedPtr<SceneTree> sceneTree() const;
	

#ifdef _DEBUG
	EditorCamera3D *editor_camera_ = nullptr;
#endif
	
	inipp::Ini<char> config_;
private:
	SharedPtr<Window> window_;
};