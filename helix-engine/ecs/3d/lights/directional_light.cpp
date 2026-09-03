#include "directional_light.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui_internal.h"
#include "math.hpp"
#include "ecs/transform.h"
#include "ecs/3d/camera.hpp"
#include "ecs/core/scene_tree.hpp"

ComponentProvider<DirectionalLight> ComponentProvider<DirectionalLight>::instance_ = ComponentProvider();

namespace detail {
	
	mat4 calculate_light_space_matrix(Camera3D const *cam, Component const *This, float const nearPlane, float const farPlane, float zMult) {
		mat4 const proj = glm::perspective(cam->get_field_of_vision(), cam->get_aspect_ratio(), nearPlane, farPlane);
		Vector<vec4> const corners = frustum_corners_world_space(glm::inverse(proj * cam->get_view()));

        vec3 center(0);
        for (auto const & v : corners)
        {
            center += vec3(v);
        }
        center /= corners.size();

        auto const &transform = This->get_entity()->get_component<Transform>();
		SharedPtr<SceneTree> st = This->tree.lock();
		Entity*tr = st->get_entity(2);
		vec3 tr_pos = tr->get_component<Transform>().translation;
		Entity*sc = st->get_entity(3);
		vec3 sc_pos = sc->get_component<Transform>().translation;

		vec3 lightDir = glm::normalize(tr_pos - sc_pos);
		
        mat4 const lightView = glm::lookAt(
	        center - lightDir,
	        center,
	        vec3(0, 1, 0)
		);
		
        f32 minX = std::numeric_limits<f32>::max();
        f32 maxX = std::numeric_limits<f32>::lowest();
        f32 minY = std::numeric_limits<f32>::max();
        f32 maxY = std::numeric_limits<f32>::lowest();
        f32 minZ = std::numeric_limits<f32>::max();
        f32 maxZ = std::numeric_limits<f32>::lowest();

		for (auto const & v : corners)
        {
	        vec4 const trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // Tune this parameter according to the scene
/*
		auto temp = -minZ;
		minZ = -maxZ;
		maxZ = temp;
		auto mid = (maxZ - minZ) / 2;
		minZ -= mid * 5.0f;
		maxZ += mid * 5.0f;
		
*/
		if (minZ < 0)
		{
			minZ *= zMult;
		}
		else
		{
			minZ /= zMult;
		}
		if (maxZ < 0)
		{
			maxZ /= zMult;
		}
		else
		{
			maxZ *= zMult;
		}
		
        mat4 const lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        return lightProjection * lightView;
    }
	Vector<mat4> calculate_light_space_matrices(Camera3D const *cam, Component const *This, Vector<f32> const &shadowCascadeLevels, f32 zMult) {
		Vector<mat4> ret;
		for (size_t i = 0; i < shadowCascadeLevels.size() + 1; i++)
			if (i == 0)
				ret.push_back(calculate_light_space_matrix(cam, This, cam->get_near_plane(), shadowCascadeLevels[i], zMult));
			else if (i < shadowCascadeLevels.size())
				ret.push_back(calculate_light_space_matrix(cam, This, shadowCascadeLevels[i - 1], shadowCascadeLevels[i], zMult));
			else
				ret.push_back(calculate_light_space_matrix(cam, This, shadowCascadeLevels[i - 1], cam->get_far_plane(), zMult));

		return ret;
	}
} // namespace detail

void DirectionalLight::reset_cascade_view() {
	vc0 = false;
	vc1 = false;
	vc2 = false;
	vc3 = false;
	vc4 = false;
}

DirectionalLight::DirectionalLight() : cascade_count_(0) {
}

DirectionalLight::DirectionalLight(Weak<SceneTree> const &scene_tree, const RID ent) : Component(scene_tree, ent), cascade_count_(3u) {
	

	rebuild();
}

u8 DirectionalLight::get_cascades() const { return cascade_count_; }
RID DirectionalLight::get_texture() const { return tx_; }
RID DirectionalLight::get_buffer() const { return lsm_; }

void DirectionalLight::set_cascades(u8 const cascades) {
	cascade_count_ = cascades;
}

Optional<RenderPassInfo> DirectionalLight::get_custom_render_pass() const
{
	return std::nullopt;
} 

void DirectionalLight::render_setup(RenderPassInfo const &info) {
	
}

void DirectionalLight::editor() {
	using namespace ImGui;

	Checkbox("Inspect Light", &inspect);

	if (inspect) {
		if (Begin(std::format("Directional Light [{}]", get_entity()->get_id().upper).c_str())) {
			if (Button("View from the first cascade")) {
				reset_cascade_view();
				vc0 = true;
			}
			if (Button("View from the second cascade")) {
				reset_cascade_view();
				vc1 = true;
			}
			if (Button("View from the third cascade")) {
				reset_cascade_view();
				vc2 = true;
			}
			if (Button("View from the fourth cascade")) {
				reset_cascade_view();
				vc3 = true;
			}
			if (Button("View from the fifth cascade")) {
				reset_cascade_view();
				vc4 = true;
			}
		}
		End();
	}
}

void DirectionalLight::rebuild() {
}

