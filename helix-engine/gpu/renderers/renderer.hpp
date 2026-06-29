#pragma once

#include "types.hpp"
#include "math.hpp"
#include "engine/disposable.hpp"

class Entity;
class Mesh;
class Window;
class SceneTree;

//
// Defines the necessary methods for a renderer.
//
class IRenderer : public IDisposable {
public:
	IRenderer(SharedPtr<Window> const &) {}
	~IRenderer() override = default;

	virtual Result<> resize(ivec2) = 0;
	virtual Result<> render() = 0;
	
	virtual [[nodiscard]] SharedPtr<SceneTree> sceneTree() const = 0;
};