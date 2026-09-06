

#include "lighting.hpp"

#include "driver.hpp"

LightingSystem::LightingSystem() : point_shadow_images_(0), spot_shadow_images_(0) {
	IGpuDriver* driver = GraphicsSystem::get_driver();

	const BufferDescriptor point_lights = gfx::buffer<PointLight>("Points Lights", MAX_POINT_LIGHTS, gfx::BufferUsage::eShaderDeviceAddress);
	point_light_buffer_ = driver->create_buffer(point_lights);
	driver->set_buffer_name(point_light_buffer_, "Point Lights");
	point_light_buffer_data_ = (PointLight*)driver->get_mapped_data(point_light_buffer_);
	std::memset(point_light_buffer_data_, 0, sizeof(PointLight) * MAX_POINT_LIGHTS);
	
	point_light_count = MAX_POINT_LIGHTS;
	
	ImageDescriptor image_descriptor{
		.label = "Point Shadow Image",
		.format = gfx::Format::eDepth16Unorm,
		.type = gfx::ImageType::e3D,
		.usage = gfx::ImageUsage::eDepthStencilAttachment | gfx::ImageUsage::eSampled,
		.size = uint3(POINT_SHADOW_RESOLUTION, POINT_SHADOW_RESOLUTION, 6),
		.array_layers = MAX_POINT_SHADOWS,
		.mip_levels = 1
	};
	
	for (std::size_t i = 0; i < MAX_POINT_SHADOWS; ++i) {
		point_shadow_stack_.push(static_cast<int>(MAX_POINT_SHADOWS - (i + 1)));
	}

	for (std::size_t i = 0; i < MAX_POINT_LIGHTS; ++i)
		point_light_stack_.push(static_cast<int>(MAX_POINT_LIGHTS - (i + 1)));

	const BufferDescriptor spot_lights = gfx::buffer<SpotLight>("Spot Lights", MAX_SPOT_LIGHTS, gfx::BufferUsage::eShaderDeviceAddress);
	spot_light_buffer_ = driver->create_buffer(spot_lights);
	driver->set_buffer_name(spot_light_buffer_, "Spot Lights");
	spot_light_buffer_data_ = (SpotLight*)driver->get_mapped_data(spot_light_buffer_);
	std::memset(spot_light_buffer_data_, 0, sizeof(SpotLight) * MAX_SPOT_LIGHTS);
	
	for (std::size_t i = 0; i < MAX_SPOT_SHADOWS; ++i) {
		spot_shadow_stack_.push(MAX_SPOT_SHADOWS - i);
	}
}

LightingSystem * LightingSystem::singleton() {
	static LightingSystem singleton;
	return &singleton;
}

RID LightingSystem::get_point_shadow_program() {
	return pointShadowProgram_;
}

void LightingSystem::start_writing_point_shadows() {
	if (point_shadow_buffer_data_ != nullptr)
		return; // Already mapped for writing
}

void LightingSystem::stop_writing_point_shadows() {
	if (point_shadow_buffer_data_ == nullptr)
		return;
}

std::optional<int> LightingSystem::check_out_point_shadow() {
	if (point_shadow_stack_.empty())
		return std::nullopt;
	
	int const index = point_shadow_stack_.top();
	point_shadow_stack_.pop();
	return index;
}

void LightingSystem::check_in_point_shadow(int const index) {
	point_shadow_stack_.push(index);
}

RID LightingSystem::get_point_shadow_texture(int const index) const {
	return point_shadow_images_[index]; // supports_bindless_textures() ? *pointShadowImages[index] : *pointShadowImages.back();
}

void LightingSystem::set_point_shadow(int const index, PointShadow const &shadow) {
#ifndef _DEBUG
	assert(point_shadow_buffer_data_ != nullptr && "Point shadow buffer is not mapped for writing.");
#else
	if (point_shadow_buffer_data_ == nullptr)
		start_writing_point_shadows();
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

void LightingSystem::start_writing_point_lights() {
	if (point_light_buffer_data_ != nullptr)
		return; // Already mapped for writing
}

void LightingSystem::stop_writing_point_lights() {
	if (point_light_buffer_data_ == nullptr)
		return;
}

std::optional<int> LightingSystem::check_out_point_light() {
	assert(!point_light_stack_.empty());

	int const index = point_light_stack_.top();
	point_light_stack_.pop();
	return index;
}

void LightingSystem::check_in_point_light(int const index) {
	point_light_stack_.push(index);
}

std::optional<int> LightingSystem::check_out_spot_shadow() {
	if (spot_shadow_stack_.empty())
		return std::nullopt;
	
	int const index = spot_shadow_stack_.top();
	spot_shadow_stack_.pop();
	return index;
}

void LightingSystem::check_in_spot_shadow(int const index) {
	spot_shadow_stack_.push(index);
}

void LightingSystem::set_point_light(int index, PointLight const &light)  {
#ifdef STRICT
	assert(point_light_buffer_data_ != nullptr && "Point light buffer is not mapped for writing.");
#else
	start_writing_point_lights();
#endif
	point_light_buffer_data_[index] = light;
}

void LightingSystem::prerender() {
	stop_writing_point_shadows(); //< Prevent a possible mapped buffer being bound
	stop_writing_point_lights();  //< Prevent a possible mapped buffer being bound
}

void LightingSystem::dispose() {
	stop_writing_point_lights();
	stop_writing_point_shadows();
	IGpuDriver* driver = GraphicsSystem::get_driver();
	
	for (const RID point_shadow_image_view : point_shadow_image_views_)
		driver->destroy_image_view(point_shadow_image_view);
	
	for (const RID point_shadow_image : point_shadow_images_)
		driver->destroy_image(point_shadow_image);
	
	for (const RID spot_shadow_image_view : spot_shadow_image_views_)
		driver->destroy_image_view(spot_shadow_image_view);
	
	for (const RID spot_shadow_image : spot_shadow_images_)
		driver->destroy_image(spot_shadow_image);
	
	point_light_buffer_data_ = nullptr;
	spot_light_buffer_data_ = nullptr;
	point_shadow_buffer_data_ = nullptr;
	spot_shadow_buffer_data_ = nullptr;
	
	driver->destroy_buffer(point_light_buffer_);
	driver->destroy_buffer(point_shadow_buffer_);
	driver->destroy_buffer(spot_light_buffer_);
	driver->destroy_buffer(spot_shadow_buffer_);
	
	disposed_ = true;
}

bool LightingSystem::disposed() const {
	return disposed_;
}
