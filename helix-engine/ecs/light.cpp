#include "light.hpp"

SharedPtr<Buffer> OmniLightServer::buffer_ = nullptr;
std::size_t OmniLightServer::buffer_size_ = 0;
std::size_t OmniLightServer::count_ = 0;

ComponentServer<OmniLight> ComponentServer<OmniLight>::instance_ = ComponentServer();

void OmniLightServer::createBuffer() {
	if (!buffer_)
		buffer_ = _STD make_shared<Buffer>();
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
	constexpr auto data_size = sizeof(OmniLight::omni_light_data_t);
	buffer_->update(data_size,
		static_cast<i64>((data_size * index) + sizeof(u32)),
		&omni.data_);
}

void OmniLightServer::resize(size_t const light_count) {
	buffer_.reset(new Buffer());
	buffer_->allocStorage(sizeof(u32) + sizeof(OmniLight::omni_light_data_t) * light_count,
		nullptr, gl::BufferStorageMask::DynamicStorageBit);

	u32 const count = static_cast<u32>(light_count);
	buffer_->update(
		sizeof(u32),
		0,
		&count
	);
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

