#include "imgui.h"
#include "light.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "transform.h"
#include "gpu/lighting.hpp"

ComponentProvider<OmniLight> ComponentProvider<OmniLight>::instance_ = ComponentProvider();

void OmniLight::update(double x) {
	static RenderPassInfo ri{
		.pass = RenderPassType::Shadow,
	};

	//< Setting up for point shadow pass.
	if (!enabled_ || !shadows_enabled_ || !dirty_) return; //< 
	dirty_ = false; //< Don't re-render shadow depth if not needed.
}


void OmniLight::updatePointLight() const {
	if (!enabled_) return;
	Transform const &xform = entity.lock()->component<Transform>();
	intensity_ = intensity_ == 0.00f ? 16.0f : intensity_;
	PointLight const light{
		.Position = xform.position(),
		.Range = range_,
		.Color = color_ * intensity_,
		.ShadowMapIndex = shadows_enabled_ ? static_cast<int>(shadow_index_) : -1
	};
	LightingSystem::singleton()->setPointLight(light_index_, light);
	dirty_ = true;
}

void OmniLight::updatePointShadow() const {
	if (!shadows_enabled_ || shadow_index_ == -1)
		return;
	
	dirty_ = true; //< mark for re-render
	Transform const &xform = entity.lock()->component<Transform>();

	//< Perspective is consistent
	mat4 const proj = glm::perspective(
		90.0f,
		1.0f,
		near_,
		far_
	);
	
	//< Generate 6 directions for the cubemap shadow map
	PointShadow shadow{};

	RID texture = LightingSystem::singleton()->pointShadowTexture(shadow_index_);
	
	auto const lightProj = (mat4*)&shadow.LightViewProj;
	lightProj[0] = proj * glm::lookAt(
		xform.position(),
		xform.position() + vec3(+1.0, +0.0, +0.0),
		vec3(0.0, -1.0, 0.0)
	);
	// std::cout << "lightProj[0]\n";
	// print_matrix(lightProj[0]);
	lightProj[1] = proj * glm::lookAt(
		xform.position(),
		xform.position() + vec3(-1.0, +0.0, +0.0),
		vec3(0.0, -1.0, 0.0)
	);
	lightProj[2] = proj * glm::lookAt(
		xform.position(),
		xform.position() + vec3(+0.0, +1.0, +0.0),
		vec3(0.0, 0.0, 1.0)
	);
	lightProj[3] = proj * glm::lookAt(
		xform.position(),
		xform.position() + vec3(+0.0, -1.0, +0.0),
		vec3(0.0, 0.0, -1.0)
	);
	lightProj[4] = proj * glm::lookAt(
		xform.position(),
		xform.position() + vec3(+0.0, +0.0, +1.0),
		vec3(0.0, -1.0, 0.0)
	);
	lightProj[5] = proj * glm::lookAt(
		xform.position(),
		xform.position() + vec3(+0.0, +0.0, -1.0),
		vec3(0.0, -1.0, 0.0)
	);

	shadow.Position = xform.position();
	shadow.LightIndex = static_cast<int>(light_index_);

	shadow.NearPlane = near_;
	shadow.FarPlane  = far_;

	LightingSystem::singleton()->setPointShadow(shadow_index_, shadow);
}

OmniLight::OmniLight(Weak<SceneTree> const &scene_tree, Weak<Entity> const &ent)
	: Component(scene_tree, ent), data_({}) {}

OmniLight::~OmniLight() = default; 

bool OmniLight::dirty() const {
	return dirty_;
}

vec3 OmniLight::position() const {
	Transform const &xform = entity.lock()->component<Transform>();
	return xform.position();
}

vec3 OmniLight::color() const {
	return color_;
}

float OmniLight::range() const {
	return range_;
}

float OmniLight::intensity() const {
	return intensity_;
}

void OmniLight::setPosition(vec3 const &value) const {
	Transform &xform = entity.lock()->component<Transform>();
	xform.translation = value;
	updatePointLight();
	updatePointShadow();
}

void OmniLight::setColor(vec3 const &value) {
	color_ = value;
	updatePointLight();
	updatePointShadow();
}

void OmniLight::setRange(float const value) {
	range_ = value;
	far_ = value;
	updatePointLight();
	updatePointShadow();
}

void OmniLight::setIntensity(float const value) {
	intensity_ = value;
	updatePointLight();
	updatePointShadow();
}

void OmniLight::setEnabled(bool const enabled) {
	if (enabled == enabled_) //< State change?
		return;
	
	if (enabled) {
		auto const opt_idx = LightingSystem::singleton()->checkOutPointLight();
		if (opt_idx.has_value()) {
			light_index_ = opt_idx.value();
			updatePointLight();
			updatePointShadow();
			enabled_ = true;
		}
		else {
			light_index_ = -1;
			shadow_index_ = -1;
			enabled_ = false;
		}
	}
	else {
		LightingSystem::singleton()->checkInPointLight(light_index_);
		light_index_ = -1;
		shadow_index_ = -1;
		enabled_ = false;
	}
}

bool OmniLight::enabled() const {
	return enabled_;
}
void OmniLight::setShadowsEnabled(bool const enabled) {
	if (enabled == shadows_enabled_)
		return;

	if (enabled) {
		auto const opt_idx = LightingSystem::singleton()->checkOutPointShadow();
		if (opt_idx.has_value()) {
			shadow_index_ = opt_idx.value();
			shadows_enabled_ = true;
			updatePointLight();
			updatePointShadow();
		}
	}
	else {
		RID texture = LightingSystem::singleton()->pointShadowTexture(shadow_index_);
		LightingSystem::singleton()->checkInPointShadow(shadow_index_);
		updatePointLight();
		updatePointShadow();
		shadows_enabled_ = false;
		shadow_index_ = -1;
	}
}
bool OmniLight::shadowsEnabled() const {
	return shadows_enabled_;
}

void OmniLight::editor() {
	using namespace ImGui;

	if (Checkbox("Enabled", &enabled_)) {
		if (enabled_) {
			auto const opt_idx = LightingSystem::singleton()->checkOutPointLight();
			if (opt_idx.has_value()) {
				light_index_ = opt_idx.value();
				updatePointLight();
				updatePointShadow();
				enabled_ = true;
			}
			else {
				light_index_ = -1;
				shadow_index_ = -1;
				enabled_ = false;
			}
		}
		else {
			LightingSystem::singleton()->checkInPointLight(light_index_);
			light_index_ = -1;
			shadow_index_ = -1;
			enabled_ = false;
		}
	}

	if (enabled()) {
		if (Checkbox("Shadows Enabled", &shadows_enabled_)) {
			if (shadows_enabled_) {
				auto const opt_idx = LightingSystem::singleton()->checkOutPointShadow();
				if (opt_idx.has_value()) {
					shadow_index_ = opt_idx.value();
					updatePointLight();
					updatePointShadow();
				}
				else {
					throw std::exception("OmniLight: No available point shadow slots to enable shadows for this light. Consider increasing the maximum point shadow count in LightingSystem.");
				}
			}
			else {
				RID texture = LightingSystem::singleton()->pointShadowTexture(shadow_index_);
				LightingSystem::singleton()->checkInPointShadow(shadow_index_);
				shadow_index_ = -1;
				updatePointLight();
				updatePointShadow();
			}
		}
		if (shadowsEnabled()) {
			if (ColorEdit3("Color", &color_[0])) { updatePointLight(); updatePointShadow(); }
			if (SliderFloat("Intensity", &intensity_, 0.0f, 64.0f)) { updatePointLight(); updatePointShadow(); }
			if (SliderFloat("Range", &range_, 0.0f, 64.0f)) { updatePointLight(); updatePointShadow(); }
			if (SliderFloat("Near Plane", &near_, 0.01f, far_ - 0.01f)) { updatePointShadow(); }
			if (SliderFloat("Far Plane", &far_, near_ + 0.01f, 128.0f)) { updatePointShadow(); }
		}
	}
}
