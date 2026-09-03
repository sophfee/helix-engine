#pragma once

#include "ecs.hpp"
#include "math.hpp"
#include "core/component.hpp"

class Buffer;
class OmniLightServer;

class OmniLight : public Component {

	static constexpr auto OMNI_LIGHT_SHADOW_RESOLUTION = 2048;
	
	int light_index_ = -1;
	int shadow_index_ = -1;
	
	vec3 color_ = vec3(1.0, 1.0, 1.0);
	mutable float intensity_ = 1.0f;

	float range_ = 10.0f;

	float near_ = 0.01f, far_ = 64.0f;

	bool enabled_ = false;
	bool shadows_enabled_ = false;

	void update_point_light() const;
	void update_point_shadow() const;
	
public:
	OmniLight();
	OmniLight(Weak<SceneTree> const &scene_tree, const RID ent);
	~OmniLight() override;

	_NODISCARD bool dirty() const;

	_NODISCARD vec3 get_position() const;
	_NODISCARD vec3 get_color() const;
	_NODISCARD float get_range() const;
	_NODISCARD float get_intensity() const;

	void set_position(vec3 const &value);
	void set_color(vec3 const &value);
	void set_range(float value);
	void set_intensity(float value);

	void set_enabled(bool enabled);
	_NODISCARD bool is_enabled() const;

	void set_shadows_enabled(bool enabled);
	_NODISCARD bool get_shadows_enabled() const;

	void editor() override;
	
	struct OmniLightStorage {
		vec3 position;
		float intensity;
		vec3 color;
		float range;
	};

	void update(double) override;

	mutable OmniLightStorage data_;
	mutable bool dirty_ = false;

public:
	friend class OmniLightServer;
};
