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

	void updatePointLight() const;
	void updatePointShadow() const;
	
public:
	OmniLight(Weak<SceneTree> const &scene_tree, Weak<Entity> const &ent);
	~OmniLight() override;

	_NODISCARD bool dirty() const;

	_NODISCARD vec3 position() const;
	_NODISCARD vec3 color() const;
	_NODISCARD float range() const;
	_NODISCARD float intensity() const;

	void setPosition(vec3 const &value) const;
	void setColor(vec3 const &value);
	void setRange(float value);
	void setIntensity(float value);

	void setEnabled(bool enabled);
	_NODISCARD bool enabled() const;

	void setShadowsEnabled(bool enabled);
	_NODISCARD bool shadowsEnabled() const;

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

class OmniLightServer final {
public:
	static void bindBuffer(int base);
	static void upload(size_t const index, OmniLight const &omni);
	static void resize(size_t const light_count);
	static void createBuffer();

	static float *beginWrite();
	static void endWrite();
	
	static void resetCount();
	static std::size_t incrementCount();
	
	static std::size_t count_;
	static SharedPtr<Buffer> buffer_;
	static std::size_t buffer_size_;
private:
};