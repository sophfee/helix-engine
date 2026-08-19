#pragma once

#include "types.hpp"
#include "math.hpp"
#include "engine/disposable.hpp"
#include "engine/rid.hpp"
#include "gpu/geometry.hpp"

class Entity;
class Mesh;
class Window;
class SceneTree;

enum class RendererType {
	DEFERRED,
	FORWARD,
	FORWARD_MULTI
};

struct SceneData {
	float4x4 view;
	float4x4 proj;
	float4x4 proj_view;

	float4x4 frozen_view;
	float4x4 frozen_proj;
	float4x4 frozen_proj_view;

	Frustum frustum;
	Frustum frozen_frustum;

	float4 camera_world_position;
	float4 frozen_camera_world_position;

	float time;
	float delta_time;
};

//
// Defines the necessary methods for a renderer.
//
class IRenderer : public IDisposable {
public:
	IRenderer(SharedPtr<Window> const &) {}
	~IRenderer() override = default;

	virtual [[nodiscard]] Result<> resize(ivec2) = 0;
	virtual [[nodiscard]] Result<> render() = 0;

	virtual [[nodiscard]] RendererType rendererType() const = 0;
	
	virtual [[nodiscard]] RID primaryBindGroupLayout() const = 0;
	virtual void requestNewFrame() = 0;
	
	virtual [[nodiscard]] const SceneData &sceneData() const = 0;
	virtual [[nodiscard]] SceneData &sceneDataMut() = 0;
	virtual [[nodiscard]] RID sceneDataRid() const = 0;
	
	virtual [[nodiscard]] SharedPtr<SceneTree> sceneTree() const = 0;
};