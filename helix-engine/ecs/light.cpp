#include "imgui.h"
#include "light.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "gpu/buffer.h"

#include "math.hpp"
#include "transform.h"
#include "gpu/framebuffer.h"
#include "gpu/lighting.hpp"

SharedPtr<Buffer> OmniLightServer::buffer_ = nullptr;
std::size_t OmniLightServer::buffer_size_ = 0;
std::size_t OmniLightServer::count_ = 0;

ComponentProvider<OmniLight> ComponentProvider<OmniLight>::instance_ = ComponentProvider();

void OmniLightServer::createBuffer() {
	resize(64);
}

float *OmniLightServer::beginWrite() {
	if (!buffer_)createBuffer();
	return static_cast<float *>(buffer_->map(gl::BufferAccessARB::WriteOnly));
}
void OmniLightServer::endWrite() {
	assert(buffer_ && "Cannot call endWrite() without a pre-existing buffer. Either something is terribly wrong or you are silly."); // CANNOT CALL THIS WITHOUT A BUFFER YOU CRAZY!
	assert(buffer_->unmap() && "Failed to unmap buffer after writing omni light data.");
}

void OmniLightServer::resetCount() {
	count_ = 0;
}

std::size_t OmniLightServer::incrementCount() {
	return ++count_;
}

void OmniLight::update(double x) {
	static RenderPassInfo ri{
		.pass = RenderPassType::Shadow,
		.model_matrix_location = 0,
		.cull = true,
		.cull_face = gl::TriangleFace::Back,
		.viewport = ivec4(0, 0, OMNI_LIGHT_SHADOW_RESOLUTION, OMNI_LIGHT_SHADOW_RESOLUTION)
	};

	//< Setting up for point shadow pass.
	if (!enabled_ || !shadows_enabled_ || !dirty_) return;
	dirty_ = false; //< Don't re-render shadow depth if not needed.

	Program &prog = LightingSystem::singleton()->pointShadowProgram();
	ri.shader_program = &prog;

	prog.use();
	prog.setUniform("lightIndex", shadow_index_);

	LightingSystem::singleton()->stopWritingPointShadows(); // unmap before draw
	Framebuffer const &fb = LightingSystem::singleton()->pointShadowFramebuffer(shadow_index_);
	fb.bind();
	glClear(GL_DEPTH_BUFFER_BIT); // <-- are you clearing the depth buffer before drawing?
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);         // <-- is depth writing actually enabled?
	sceneTree()->initiateDraw(ri);
	fb.unbind();
}

void OmniLightServer::bindBuffer(int const base) {
	if (!buffer_) createBuffer();
	buffer_->bindBufferBase(gl::BufferTargetARB::ShaderStorageBuffer, base);
}

void OmniLightServer::upload(size_t const index, OmniLight const &omni) {
	if (!buffer_) createBuffer();
	constexpr auto data_size = sizeof(OmniLight::OmniLightStorage);

	using enum gl::MapBufferAccessMask;
	auto const light = (OmniLight::OmniLightStorage *)buffer_->mapRange(
		static_cast<i64>(data_size * index),
		data_size, MapWriteBit
	);
	*light = omni.data_;
	assert(buffer_->unmap() && "Failed to unmap buffer after uploading omni light data.");
}

void OmniLightServer::resize(size_t const light_count) {
	buffer_.reset(new Buffer());
	buffer_->allocStorage(sizeof(OmniLight::OmniLightStorage) * light_count,
	                      nullptr, gl::BufferStorageMask::MapWriteBit);
}

void OmniLight::updatePointLight() const {
	if (!enabled_) return;
	Transform const &xform = entity.lock()->component<Transform>();
	intensity_ = intensity_ == 0.00f ? 2.0f : intensity_;
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
	mat4 proj = glm::perspective(
		90.0f,
		1.0f,
		near_,
		far_
	);

	// print_matrix(proj);

	//< Generate 6 directions for the cubemap shadow map
	PointShadow shadow{};

	Texture const &texture = LightingSystem::singleton()->pointShadowTexture(shadow_index_);
	texture.makeResident();

	shadow.ShadowTexture = texture.textureHandle();
			
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
			printf("OmniLight: No available point light slots to enable this light. Consider increasing the maximum point light count in LightingSystem.\n");
			enabled_ = false;
		}
	}
	else {
		LightingSystem::singleton()->checkInPointLight(light_index_);
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
		Texture const &texture = LightingSystem::singleton()->pointShadowTexture(shadow_index_);
		texture.makeNonResident();
		LightingSystem::singleton()->checkInPointShadow(shadow_index_);
		shadows_enabled_ = false;
		updatePointLight();
		updatePointShadow();
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
				printf("OmniLight: No available point light slots to enable this light. Consider increasing the maximum point light count in LightingSystem.\n");
				enabled_ = false;
			}
		}
		else {
			LightingSystem::singleton()->checkInPointLight(light_index_);
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
			}
			else {
				Texture const &texture = LightingSystem::singleton()->pointShadowTexture(shadow_index_);
				texture.makeNonResident();
				LightingSystem::singleton()->checkInPointShadow(shadow_index_);
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
