#pragma once

#include "types.hpp"
#include "math.hpp"
#include "engine/disposable.hpp"
#include "engine/rid.hpp"

class Entity;
class Mesh;
class Window;
class SceneTree;

enum class RendererType {
	DEFERRED,
	FORWARD,
	FORWARD_MULTI
};

//
// Defines the necessary methods for a renderer.
//
class IRenderer : public IDisposable {
public:
	IRenderer(SharedPtr<Window> const &) {}
	~IRenderer() override = default;

	virtual Result<> resize(ivec2) = 0;
	virtual Result<> render() = 0;

	virtual RendererType rendererType() const = 0;
	
	virtual RID primaryBindGroupLayout() const = 0;
	virtual void requestNewFrame() = 0;
	
	virtual [[nodiscard]] SharedPtr<SceneTree> sceneTree() const = 0;
};