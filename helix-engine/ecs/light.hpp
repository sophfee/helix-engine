#pragma once

#include "ecs.hpp"
#include "math.hpp"

class COmniLightServer;

class COmniLight : public Component {
public:
	COmniLight(CWeakPtr<CSceneTree> const &scene_tree, CWeakPtr<CEntity> const &ent);
	~COmniLight() override;

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
	friend class COmniLightServer;
};

class COmniLightServer final {
public:
	static void bindBuffer(int base);
	static void upload(size_t const index, COmniLight const &omni);
	static void resize(size_t const light_count);
	static void createBuffer();
	
	static void resetCount();
	static std::size_t incrementCount();
	
	static std::size_t count_;
	static CSharedPtr<CBuffer> buffer_;
	static std::size_t buffer_size_;
private:
};