#include "directional_light.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui_internal.h"
#include "math.hpp"
#include "ecs/transform.h"
#include "ecs/3d/camera.hpp"

ComponentProvider<DirectionalLight> ComponentProvider<DirectionalLight>::instance_ = ComponentProvider();

namespace detail {
	
	mat4 calculateLightSpaceMatrix(Camera3D const *cam, Component const *This, float const nearPlane, float const farPlane, float zMult) {
		mat4 const proj = glm::perspective(cam->fieldOfVision(), cam->aspectRatio(), nearPlane, farPlane);
		Vec<vec4> const corners = frustumCornersWorldSpace(glm::inverse(proj * cam->viewMatrix()));

        vec3 center(0);
        for (auto const & v : corners)
        {
            center += vec3(v);
        }
        center /= corners.size();

        auto const &transform = This->entity.lock()->component<Transform>();
		SharedPtr<SceneTree> st = This->tree.lock();
		SharedPtr<Entity> tr = st->entity(2);
		vec3 tr_pos = tr->component<Transform>().translation;
		SharedPtr<Entity> sc = st->entity(3);
		vec3 sc_pos = sc->component<Transform>().translation;

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
	Vec<mat4> calculateLightSpaceMatrices(Camera3D const *cam, Component const *This, Vec<f32> const &shadowCascadeLevels, f32 zMult) {
		Vec<mat4> ret;
		for (size_t i = 0; i < shadowCascadeLevels.size() + 1; i++)
			if (i == 0)
				ret.push_back(calculateLightSpaceMatrix(cam, This, cam->nearPlane(), shadowCascadeLevels[i], zMult));
			else if (i < shadowCascadeLevels.size())
				ret.push_back(calculateLightSpaceMatrix(cam, This, shadowCascadeLevels[i - 1], shadowCascadeLevels[i], zMult));
			else
				ret.push_back(calculateLightSpaceMatrix(cam, This, shadowCascadeLevels[i - 1], cam->farPlane(), zMult));

		return ret;
	}
} // namespace detail

void DirectionalLight::resetCascadeView() {
	vc0 = false;
	vc1 = false;
	vc2 = false;
	vc3 = false;
	vc4 = false;
}
DirectionalLight::DirectionalLight(Weak<SceneTree> const &scene_tree, Weak<Entity> const &ent) : Component(scene_tree, ent), cascade_count_(3u) {
	

	rebuild();
}

u8 DirectionalLight::cascades() const { return cascade_count_; }
RID DirectionalLight::texture() const { return tx_; }
RID DirectionalLight::buffer() const { return lsm_; }

void DirectionalLight::setCascades(u8 const cascades) {
	cascade_count_ = cascades;
}

Optional<RenderPassInfo> DirectionalLight::customRenderPass() const
{
	return std::nullopt;
} 

void DirectionalLight::renderSetup(RenderPassInfo const &info) {
	
}

void DirectionalLight::editor() {
	using namespace ImGui;

	Checkbox("Inspect Light", &inspect);

	if (inspect) {
		if (Begin(std::format("Directional Light [{}]", entity.lock()->id()).c_str())) {
			if (Button("View from the first cascade")) {
				resetCascadeView();
				vc0 = true;
			}
			if (Button("View from the second cascade")) {
				resetCascadeView();
				vc1 = true;
			}
			if (Button("View from the third cascade")) {
				resetCascadeView();
				vc2 = true;
			}
			if (Button("View from the fourth cascade")) {
				resetCascadeView();
				vc3 = true;
			}
			if (Button("View from the fifth cascade")) {
				resetCascadeView();
				vc4 = true;
			}
		}
		End();
	}
}

void DirectionalLight::rebuild() {
}

