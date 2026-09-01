

#include "lighting.hpp"

#include "driver.hpp"

LightingSystem::LightingSystem()
	: point_shadow_images_(0),
	  spot_shadow_images_(0) {
	GraphicsBackend* backend = GraphicsDriver::get();

	const BufferDescriptor point_lights = gfx::buffer<PointLight>("Points Lights", MAX_POINT_LIGHTS, gfx::BufferUsage::eShaderDeviceAddress);
	point_light_buffer_ = backend->CreateBuffer(point_lights);
	backend->SetBufferName(point_light_buffer_, "Point Lights");
	point_light_buffer_data_ = (PointLight*)backend->GetMappedData(point_light_buffer_);
	
	point_light_count = MAX_POINT_LIGHTS;
	
	for (int i = 0; i < MAX_POINT_SHADOWS; ++i) {
		point_shadow_stack_.push(static_cast<int>(MAX_POINT_SHADOWS - i));
	}
	
	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
		point_light_stack_.push(static_cast<int>(MAX_POINT_LIGHTS - i));
	}
	
	const BufferDescriptor spot_lights = gfx::buffer<SpotLight>("Spot Lights", MAX_SPOT_LIGHTS, gfx::BufferUsage::eShaderDeviceAddress);
	spot_light_buffer_ = backend->CreateBuffer(spot_lights);
	backend->SetBufferName(spot_light_buffer_, "Spot Lights");
	spot_light_buffer_data_ = (SpotLight*)backend->GetMappedData(spot_light_buffer_);
	
	for (int i = 0; i < MAX_SPOT_SHADOWS; ++i){
		spot_shadow_stack_.push(static_cast<int>(MAX_SPOT_SHADOWS - i));
	}
}

LightingSystem * LightingSystem::singleton() {
	static LightingSystem singleton;
	return &singleton;
}

RID LightingSystem::pointShadowProgram() {
	return pointShadowProgram_;
}

void LightingSystem::startWritingPointShadows() {
	if (point_shadow_buffer_data_ != nullptr)
		return; // Already mapped for writing
}

void LightingSystem::stopWritingPointShadows() {
	if (point_shadow_buffer_data_ == nullptr)
		return;
}

std::optional<int> LightingSystem::checkOutPointShadow() {
	if (point_shadow_stack_.empty())
		return std::nullopt;
	
	int const index = point_shadow_stack_.top();
	point_shadow_stack_.pop();
	return index;
}

void LightingSystem::checkInPointShadow(int const index) {
	point_shadow_stack_.push(index);
}

RID LightingSystem::pointShadowTexture(int const index) const {
	return point_shadow_images_[index]; // supports_bindless_textures() ? *pointShadowImages[index] : *pointShadowImages.back();
}

void LightingSystem::setPointShadow(int const index, PointShadow const &shadow) {
#ifndef _DEBUG
	assert(point_shadow_buffer_data_ != nullptr && "Point shadow buffer is not mapped for writing.");
#else
	if (point_shadow_buffer_data_ == nullptr)
		startWritingPointShadows();
#endif

#ifdef _DEBUG
	
	// std::cout << "Setting point shadow at index " << index << ":\n";
	// std::cout << "  Shadow Texture Handle: " << shadow.ShadowTexture << "\n";
	// std::cout << "  Position: " << shadow.Position.x << " " << shadow.Position.y << " " << shadow.Position.z << "\n";
	// std::cout << "  Light Index: " << shadow.LightIndex << "\n";
	// std::cout << "  Near Plane: " << shadow.NearPlane << "\n";
	// std::cout << "  Far Plane: " << shadow.FarPlane << "\n";
	

	// printf("LightViewProj[0][0][0] = %f\n", shadow.LightViewProj[0][0][0]);
	// printf("Position = %f %f %f\n", shadow.Position.x, shadow.Position.y, shadow.Position.z);
	
#endif
	point_shadow_buffer_data_[index] = shadow;
}

void LightingSystem::startWritingPointLights() {
	if (point_light_buffer_data_ != nullptr)
		return; // Already mapped for writing
}

void LightingSystem::stopWritingPointLights() {
	if (point_light_buffer_data_ == nullptr)
		return;
}

std::optional<int> LightingSystem::checkOutPointLight() {
	assert(!point_light_stack_.empty());

	int const index = point_light_stack_.top();
	point_light_stack_.pop();
	return index;
}

void LightingSystem::checkInPointLight(int const index) {
	point_light_stack_.push(index);
}

std::optional<int> LightingSystem::checkOutSpotShadow() {
	if (spot_shadow_stack_.empty())
		return std::nullopt;
	
	int const index = spot_shadow_stack_.top();
	spot_shadow_stack_.pop();
	return index;
}

void LightingSystem::checkInSpotShadow(int const index) {
	spot_shadow_stack_.push(index);
}

void LightingSystem::setPointLight(int index, PointLight const &light)  {
#ifdef STRICT
	assert(point_light_buffer_data_ != nullptr && "Point light buffer is not mapped for writing.");
#else
	startWritingPointLights();
#endif
	point_light_buffer_data_[index] = light;
}

void LightingSystem::prerender() {
	stopWritingPointShadows(); //< Prevent a possible mapped buffer being bound
	stopWritingPointLights();  //< Prevent a possible mapped buffer being bound
}

void LightingSystem::dispose() {
	stopWritingPointLights();
	stopWritingPointShadows();
	GraphicsBackend* driver = GraphicsDriver::get();
	
	for (const RID point_shadow_image_view : point_shadow_image_views_)
		driver->DestroyImageView(point_shadow_image_view);
	
	for (const RID point_shadow_image : point_shadow_images_)
		driver->DestroyImage(point_shadow_image);
	
	for (const RID spot_shadow_image_view : spot_shadow_image_views_)
		driver->DestroyImageView(spot_shadow_image_view);
	
	for (const RID spot_shadow_image : spot_shadow_images_)
		driver->DestroyImage(spot_shadow_image);
	
	point_light_buffer_data_ = nullptr;
	spot_light_buffer_data_ = nullptr;
	point_shadow_buffer_data_ = nullptr;
	spot_shadow_buffer_data_ = nullptr;
	
	driver->DestroyBuffer(point_light_buffer_);
	driver->DestroyBuffer(point_shadow_buffer_);
	driver->DestroyBuffer(spot_light_buffer_);
	driver->DestroyBuffer(spot_shadow_buffer_);
	
	disposed_ = true;
}

bool LightingSystem::disposed() const {
	return disposed_;
}
