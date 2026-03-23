#include "light.hpp"

CSharedPtr<CBuffer> COmniLightServer::buffer_ = nullptr;
std::size_t COmniLightServer::buffer_size_ = 0;
std::size_t COmniLightServer::count_ = 0;

CComponentServer<COmniLight> CComponentServer<COmniLight>::instance_ = CComponentServer();

void COmniLightServer::createBuffer() {
	if (!buffer_)
		buffer_ = _STD make_shared<CBuffer>();
}

void COmniLightServer::resetCount() {
	count_ = 0;
}

std::size_t COmniLightServer::incrementCount() {
	return ++count_;
}

void COmniLightServer::bindBuffer(int const base) {
	if (!buffer_) createBuffer();
	buffer_->bindBufferBase(gl::BufferTargetARB::ShaderStorageBuffer, base);
}

void COmniLightServer::upload(size_t const index, COmniLight const &omni) {
	if (!buffer_) createBuffer();
	constexpr auto data_size = sizeof(COmniLight::omni_light_data_t);
	buffer_->update(data_size,
		static_cast<i64>((data_size * index) + sizeof(u32)),
		&omni.data_);
}

void COmniLightServer::resize(size_t const light_count) {
	buffer_.reset(new CBuffer());
	buffer_->allocStorage(sizeof(COmniLight::omni_light_data_t) * light_count,
		nullptr, gl::BufferStorageMask::DynamicStorageBit);

	u32 const count = static_cast<u32>(light_count);
	buffer_->update(
		sizeof(u32),
		0,
		&count
	);
}

COmniLight::COmniLight(CWeakPtr<CSceneTree> const &scene_tree, CWeakPtr<CEntity> const &ent)
	: Component(scene_tree, ent), data_({}) {}

COmniLight::~COmniLight() = default; 

bool COmniLight::dirty() const {
	return dirty_;
}

vec3 COmniLight::position() const {
	return data_.position;
}

vec3 COmniLight::color() const {
	return data_.color;
}

float COmniLight::range() const {
	return data_.range;
}

float COmniLight::intensity() const {
	return data_.intensity;
}

void COmniLight::setPosition(vec3 const &value) {
	data_.position = value;
	dirty_ = true;
}

void COmniLight::setColor(vec3 const &value) {
	data_.color = value;
	dirty_ = true;
}

void COmniLight::setRange(float const value) {
	data_.range = value;
	dirty_ = true;
}

void COmniLight::setIntensity(float const value) {
	data_.intensity = value;
	dirty_ = true;
}

