#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "ecs/core/component.hpp"
#include "gpu/graphics.hpp"

class Buffer;
class Framebuffer;
class Texture;
class Environment;

namespace detail {
    inline Vec<vec4> frustumCornersWorldSpace(mat4 const &projView) {
	    mat4 const inv = glm::inverse(projView);

    	Vec<vec4> frustumCorners;
    	for (u32 ix = 0; ix < 2; ++ix)
    		for (u32 iy = 0; iy < 2; ++iy)
    			for (u32 iz = 0; iz < 2; ++iz) {
    				f32  const x = static_cast<f32>(ix);
    				f32  const y = static_cast<f32>(iy);
    				f32  const z = static_cast<f32>(iz);
    				vec4 const pt = inv * vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
    				frustumCorners.push_back(pt / pt.w);
    			}
    	
    	return frustumCorners;
    }
	
    inline Vec<vec4> frustumCornersWorldSpace(mat4 const &proj, mat4 const &view) {
	    return frustumCornersWorldSpace(proj * view);
    }

    mat4 calculateLightSpaceMatrix(Camera3D const *cam, Component const *This, f32 nearPlane, f32 farPlane, f32 zMult);
	Vec<mat4> calculateLightSpaceMatrices(Camera3D const *cam, Component const *This, Array<f32, 4> const &shadowCascadeLevels, f32 zMult);
}

class DirectionalLight : public Component {

	static SharedPtr<Program> render_depth_;

	Box<Framebuffer> fb_;
	Box<Texture> tx_;
	Box<Buffer> lsm_;
	u8 cascade_count_;
	f32 zMult = 20.0f;
	
public:
	DirectionalLight(Weak<SceneTree> const &scene_tree, Weak<Entity> const &ent);

	[[nodiscard]] u8 cascades() const;
	[[nodiscard]] Box<Framebuffer> const &framebuffer() const;
	[[nodiscard]] Box<Texture> const &texture() const;
	[[nodiscard]] Box<Buffer> const &buffer() const;

	void setCascades(u8 cascades);

	[[nodiscard]] Optional<RenderPassInfo> customRenderPass() const override;
	void renderSetup(RenderPassInfo const &info) override;

	void editor() override;

protected:

	void rebuild();

public:
	friend class Environment;
};