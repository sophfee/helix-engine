#include "directional_light.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui_internal.h"
#include "math.hpp"
#include "ecs/transform.h"
#include "ecs/3d/camera.hpp"
#include "gpu/buffer.h"
#include "gpu/texture.h"

SharedPtr<Program> DirectionalLight::render_depth_ = nullptr;
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
		SharedPtr<Entity> sc = st->entity(127);
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
	if (render_depth_ == nullptr)
		render_depth_ = std::make_shared<Program>(
			"shaders\\csm.vert",
			"shaders\\csm.geom",
			"shaders\\csm.frag"
		);

	lsm_.reset(new Buffer());
	lsm_->allocStorage(sizeof(mat4) * 16 + sizeof(f32) * 16, nullptr, gl::BufferStorageMask::DynamicStorageBit);
	lsm_->setLabel("DirectionalLight LSM Buffer");

	rebuild();
}

u8 DirectionalLight::cascades() const { return cascade_count_; }
Box<Framebuffer> const & DirectionalLight::framebuffer() const { return fb_; }
Box<Texture> const & DirectionalLight::texture() const { return tx_; }
Box<Buffer> const & DirectionalLight::buffer() const { return lsm_; }

void DirectionalLight::setCascades(u8 const cascades) {
	cascade_count_ = cascades;
}

Optional<RenderPassInfo> DirectionalLight::customRenderPass() const
{
	using enum gl::BufferTargetARB;
	
	fb_->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	Camera3D const *cam = Camera3D::currentCameraEntity();
	f32 const farPlane = cam->farPlane();
	Vec<f32> const shadowCascadeLevels = { farPlane / 40.0f, farPlane / 10.0f, farPlane / 5.0f, farPlane / 2.0f };
	Vec<mat4> const lightMatrices = detail::calculateLightSpaceMatrices(cam, this, shadowCascadeLevels, zMult);
	
	lsm_->update(sizeof(mat4) * lightMatrices.size(), 0, lightMatrices.data());
	lsm_->update(sizeof(f32) * shadowCascadeLevels.size(), sizeof(mat4) * 16, shadowCascadeLevels.data());
	lsm_->bindBufferBase(ShaderStorageBuffer, 0);

	using enum RenderPassType;
	using enum gl::TriangleFace;
	return RenderPassInfo
	{
		.pass = Shadow,
		.view_matrix_location = 1,
		.projection_matrix_location = 2,
		.inverse_view_matrix_location = 3,
		.inverse_projection_matrix_location = 4,
		.bind_albedo_texture = false,
		.bind_normal_texture = false,
		.bind_orm_texture = false,
		.bind_object_id = false,
		.frustum_culling = false,
		.render_sky = false,
		.cull = true,
		.cull_face = Back,
		.bind_time = std::nullopt,
		.viewport = ivec4( 0, 0, 4096, 4096 ),
		.shader_program = render_depth_.get()
	};
}

void DirectionalLight::renderSetup(RenderPassInfo const &info) {
	if (info.pass == RenderPassType::Shadow)
		return;
	lsm_->bindBufferBase(gl::BufferTargetARB::ShaderStorageBuffer, 0);
	tx_->bindTextureUnit(6);
	glUniform1i(21, 6);
	glUniform3fv(22, 1, glm::value_ptr(vec3(entity.lock()->component<Transform>().matrix()[2])));
	glUniform1f(23, Camera3D::currentCameraEntity()->farPlane());
	glUniform3fv(24, 1, glm::value_ptr(vec3(entity.lock()->component<Transform>().translation)));
	int mode = 0;
	if (vc0) mode = 1;
	if (vc1) mode = 2;
	if (vc2) mode = 3;
	if (vc3) mode = 4;
	if (vc4) mode = 5;
	glUniform1i(25, mode);
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
	
	using enum gl::PixelType;
	using enum gl::TextureWrapMode;
	using enum gl::TextureCompareMode;
	using enum gl::CompareFunction;
	
	tx_.reset(
		new Texture(
	    Texture2DArrayBuilder()
				.resolution(ivec2{ 4096, 4096 })
				.layers(4)
				.internalFormat(gl::InternalFormat::DepthComponent32f)
				.pixelFormat(gl::PixelFormat::DepthComponent)
				.borderColor(vec4(1.0f))
				.wrapMode(ClampToBorder)
				.pixelType(Float)
				.compareMode(CompareRefToTexture)
				.compareFunc(Less)
				.filter(gl::TextureMinFilter::Linear, gl::TextureMagFilter::Linear)
		)
	);

	fb_.reset(new Framebuffer());
	fb_->attachTexture(gl::FramebufferAttachment::DepthAttachment, *tx_, 0);
	fb_->setDrawBuffers({ });
	fb_->setReadBuffer({ });
	auto status = fb_->status();
	assert(status == gl::FramebufferStatus::FramebufferComplete);
}

