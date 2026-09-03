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


void OmniLight::update_point_light() const {
	if (!enabled_) return;
	Transform const &xform = get_entity()->get_component<Transform>();
	intensity_ = intensity_ == 0.00f ? 16.0f : intensity_;
	PointLight const light{
		.position = xform.get_position(),
		.range = range_,
		.color = color_ * intensity_//,
		//.shadow_map_index = shadows_enabled_ ? static_cast<int>(shadow_index_) : -1
	};
	LightingSystem::singleton()->set_point_light(light_index_, light);
	dirty_ = true;
}

void OmniLight::update_point_shadow() const {
	if (!shadows_enabled_ || shadow_index_ == -1)
		return;
	
	dirty_ = true; //< mark for re-render
	Transform const &xform = get_entity()->get_component<Transform>();

	//< Perspective is consistent
	mat4 const proj = glm::perspective(
		90.0f,
		1.0f,
		near_,
		far_
	);
	
	//< Generate 6 directions for the cubemap shadow map
	PointShadow shadow{};

	RID texture = LightingSystem::singleton()->point_shadow_texture(shadow_index_);
	
	auto const lightProj = (mat4*)&shadow.LightViewProj;
	lightProj[0] = proj * glm::lookAt(
		xform.get_position(),
		xform.get_position() + vec3(+1.0, +0.0, +0.0),
		vec3(0.0, -1.0, 0.0)
	);
	// std::cout << "lightProj[0]\n";
	// print_matrix(lightProj[0]);
	lightProj[1] = proj * glm::lookAt(
		xform.get_position(),
		xform.get_position() + vec3(-1.0, +0.0, +0.0),
		vec3(0.0, -1.0, 0.0)
	);
	lightProj[2] = proj * glm::lookAt(
		xform.get_position(),
		xform.get_position() + vec3(+0.0, +1.0, +0.0),
		vec3(0.0, 0.0, 1.0)
	);
	lightProj[3] = proj * glm::lookAt(
		xform.get_position(),
		xform.get_position() + vec3(+0.0, -1.0, +0.0),
		vec3(0.0, 0.0, -1.0)
	);
	lightProj[4] = proj * glm::lookAt(
		xform.get_position(),
		xform.get_position() + vec3(+0.0, +0.0, +1.0),
		vec3(0.0, -1.0, 0.0)
	);
	lightProj[5] = proj * glm::lookAt(
		xform.get_position(),
		xform.get_position() + vec3(+0.0, +0.0, -1.0),
		vec3(0.0, -1.0, 0.0)
	);

	shadow.Position = xform.get_position();
	shadow.LightIndex = static_cast<int>(light_index_);

	shadow.NearPlane = near_;
	shadow.FarPlane  = far_;

	LightingSystem::singleton()->set_point_shadow(shadow_index_, shadow);
}

OmniLight::OmniLight() : Component(), data_({}) {
}

OmniLight::OmniLight(Weak<SceneTree> const &scene_tree, const RID ent) : Component(scene_tree, ent), data_({}) {
}

OmniLight::~OmniLight() = default; 

bool OmniLight::dirty() const {
	return dirty_;
}

vec3 OmniLight::get_position() const {
	Transform const &xform = get_entity()->get_component<Transform>();
	return xform.get_position();
}

vec3 OmniLight::get_color() const {
	return color_;
}

float OmniLight::get_range() const {
	return range_;
}

float OmniLight::get_intensity() const {
	return intensity_;
}

void OmniLight::set_position(vec3 const &value) {
	Transform &xform = get_entity()->get_component<Transform>();
	xform.translation = value;
	update_point_light();
	update_point_shadow();
}

void OmniLight::set_color(vec3 const &value) {
	color_ = value;
	update_point_light();
	update_point_shadow();
}

void OmniLight::set_range(float const value) {
	range_ = value;
	far_ = value;
	update_point_light();
	update_point_shadow();
}

void OmniLight::set_intensity(float const value) {
	intensity_ = value;
	update_point_light();
	update_point_shadow();
}

void OmniLight::set_enabled(bool const enabled) {
	if (enabled == enabled_) //< State change?
		return;
	
	if (enabled) {
		auto const opt_idx = LightingSystem::singleton()->check_out_point_light();
		if (opt_idx.has_value()) {
			light_index_ = opt_idx.value();
			update_point_light();
			update_point_shadow();
			enabled_ = true;
		}
		else {
			light_index_ = -1;
			shadow_index_ = -1;
			enabled_ = false;
		}
	}
	else {
		LightingSystem::singleton()->check_in_point_light(light_index_);
		light_index_ = -1;
		shadow_index_ = -1;
		enabled_ = false;
	}
}

bool OmniLight::is_enabled() const {
	return enabled_;
}
void OmniLight::set_shadows_enabled(bool const enabled) {
	if (enabled == shadows_enabled_)
		return;

	if (enabled) {
		auto const opt_idx = LightingSystem::singleton()->check_out_point_shadow();
		if (opt_idx.has_value()) {
			shadow_index_ = opt_idx.value();
			shadows_enabled_ = true;
			update_point_light();
			update_point_shadow();
		}
	}
	else {
		RID texture = LightingSystem::singleton()->point_shadow_texture(shadow_index_);
		LightingSystem::singleton()->check_in_point_shadow(shadow_index_);
		update_point_light();
		update_point_shadow();
		shadows_enabled_ = false;
		shadow_index_ = -1;
	}
}
bool OmniLight::get_shadows_enabled() const {
	return shadows_enabled_;
}

void OmniLight::editor() {
	using namespace ImGui;

	if (Checkbox("Enabled", &enabled_)) {
		if (enabled_) {
			auto const opt_idx = LightingSystem::singleton()->check_out_point_light();
			if (opt_idx.has_value()) {
				light_index_ = opt_idx.value();
				update_point_light();
				update_point_shadow();
				enabled_ = true;
			}
			else {
				light_index_ = -1;
				shadow_index_ = -1;
				enabled_ = false;
			}
		}
		else {
			LightingSystem::singleton()->check_in_point_light(light_index_);
			light_index_ = -1;
			shadow_index_ = -1;
			enabled_ = false;
		}
	}

	if (is_enabled()) {
		if (Checkbox("Shadows Enabled", &shadows_enabled_)) {
			if (shadows_enabled_) {
				auto const opt_idx = LightingSystem::singleton()->check_out_point_shadow();
				if (opt_idx.has_value()) {
					shadow_index_ = opt_idx.value();
					update_point_light();
					update_point_shadow();
				}
				else {
					throw std::exception("OmniLight: No available point shadow slots to enable shadows for this light. Consider increasing the maximum point shadow count in LightingSystem.");
				}
			}
			else {
				RID texture = LightingSystem::singleton()->point_shadow_texture(shadow_index_);
				LightingSystem::singleton()->check_in_point_shadow(shadow_index_);
				shadow_index_ = -1;
				update_point_light();
				update_point_shadow();
			}
		}
		if (get_shadows_enabled()) {
			if (ColorEdit3("Color", &color_[0])) { update_point_light(); update_point_shadow(); }
			if (SliderFloat("Intensity", &intensity_, 0.0f, 64.0f)) { update_point_light(); update_point_shadow(); }
			if (SliderFloat("Range", &range_, 0.0f, 64.0f)) { update_point_light(); update_point_shadow(); }
			if (SliderFloat("Near Plane", &near_, 0.01f, far_ - 0.01f)) { update_point_shadow(); }
			if (SliderFloat("Far Plane", &far_, near_ + 0.01f, 128.0f)) { update_point_shadow(); }
		}
	}
}
