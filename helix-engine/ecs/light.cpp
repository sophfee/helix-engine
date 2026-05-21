#include "imgui.h"
#include "light.hpp"
#include "gpu/buffer.h"

#include "math.hpp"

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
																		 data_size,
																		 MapWriteBit
);
	*light = omni.data_;
	assert(buffer_->unmap() && "Failed to unmap buffer after uploading omni light data.");
}

void OmniLightServer::resize(size_t const light_count) {
	buffer_.reset(new Buffer());
	buffer_->allocStorage(sizeof(OmniLight::OmniLightStorage) * light_count,
	                      nullptr, gl::BufferStorageMask::MapWriteBit);
}

OmniLight::OmniLight(Weak<SceneTree> const &scene_tree, Weak<Entity> const &ent)
	: Component(scene_tree, ent), data_({}) {}

OmniLight::~OmniLight() = default; 

bool OmniLight::dirty() const {
	return dirty_;
}

vec3 OmniLight::position() const {
	return data_.position;
}

vec3 OmniLight::color() const {
	return data_.color;
}

float OmniLight::range() const {
	return data_.range;
}

float OmniLight::intensity() const {
	return data_.intensity;
}

void OmniLight::setPosition(vec3 const &value) {
	data_.position = value;
	dirty_ = true;
}

void OmniLight::setColor(vec3 const &value) {
	data_.color = value;
	dirty_ = true;
}

void OmniLight::setRange(float const value) {
	data_.range = value;
	dirty_ = true;
}

void OmniLight::setIntensity(float const value) {
	data_.intensity = value;
	dirty_ = true;
}
void OmniLight::editor() {
	using namespace ImGui;

	ColorEdit3("Color", &data_.color[0]);
	SliderFloat("Intensity", &data_.intensity, 0.0f, 1024.0f);
	SliderFloat("Range", &data_.range, 0.0f, 1024.0f);
}