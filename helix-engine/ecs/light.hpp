#pragma once

#include "ecs.hpp"
#include "math.hpp"
#include "core/component.hpp"

class Buffer;
class OmniLightServer;

class OmniLight : public Component {
public:
	OmniLight(Weak<SceneTree> const &scene_tree, Weak<Entity> const &ent);
	~OmniLight() override;

	_NODISCARD bool dirty() const;

	_NODISCARD vec3 position() const;
	_NODISCARD vec3 color() const;
	_NODISCARD float range() const;
	_NODISCARD float intensity() const;

	void setPosition(vec3 const &value);
	void setColor(vec3 const &value);
	void setRange(float value);
	void setIntensity(float value);
	
private:
	struct omni_light_data_t {
		vec3 position;
		vec3 color;
		float intensity;
		float range;
	};

	omni_light_data_t data_;
	bool dirty_ = false;

public:
	friend class OmniLightServer;
};

class OmniLightServer final {
public:
	static void bindBuffer(int base);
	static void upload(size_t const index, OmniLight const &omni);
	static void resize(size_t const light_count);
	static void createBuffer();
	
	static void resetCount();
	static std::size_t incrementCount();
	
	static std::size_t count_;
	static SharedPtr<Buffer> buffer_;
	static std::size_t buffer_size_;
private:
};