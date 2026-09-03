#pragma once

#include "types.hpp"
#include "math.hpp"
#include "engine/disposable.hpp"
#include "engine/rid.hpp"
#include "gpu/driver.hpp"
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
	
	GpuDeviceAddress point_lights;
	GpuDeviceAddress spot_lights;
	// GpuDeviceAddress directional_lights;
	
	float time;
	float delta_time;
	
	float2 fuckshit;
	alignas(16) float3x3 normal;
};

//
// Defines the necessary methods for a renderer.
//
class IRenderer : public IDisposable {
public:
	IRenderer(SharedPtr<Window> const &) {}
	~IRenderer() override = default;

	[[nodiscard]] virtual Result<> resize(ivec2) = 0;
	[[nodiscard]] virtual Result<> render() = 0;

	[[nodiscard]] virtual RendererType get_renderer_type() const = 0;
	
	[[nodiscard]] virtual RID get_primary_bind_group_layout() const = 0;
	virtual void request_new_frame() = 0;
	
	[[nodiscard]] virtual const SceneData &get_scene_data() const = 0;
	[[nodiscard]] virtual SceneData &get_scene_data_mutable() = 0;
	[[nodiscard]] virtual RID get_scene_data_rid() const = 0;
	
	[[nodiscard]] virtual SharedPtr<SceneTree> get_scene_tree() const = 0;
};