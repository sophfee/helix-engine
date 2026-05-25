#include <iostream>
#include "voxelizer.hpp"

#include "graphics.hpp"
#include "buffer.h"
#include "texture.h"

void Voxelizer::updateBuffer() {
	data_buffer_->update(
		sizeof(VoxelizerData),
		0,
		&settings_
	);
}

Voxelizer::Voxelizer() :
	world_grid_(new Texture(Texture3DBuilder()
		.resolution(ivec3(512))
		.pixelType(gl::PixelType::HalfFloat)
		.pixelFormat(gl::PixelFormat::Rgba)
		.internalFormat(gl::InternalFormat::Rgba16f)
		.filter(gl::TextureMinFilter::Linear, gl::TextureMagFilter::Linear))),
	program_(std::make_unique<Program>("shaders\\voxelizer.vert", "shaders\\voxelizer.frag")),
	settings_{ vec3(0.0f), vec3(1.0f) },
	data_buffer_(new Buffer())
{
	program_->setLabel("Voxelizer Program");

	std::string str = program_->linkLog();
	std::cout << str << '\n';
	
	data_buffer_->allocStorage(sizeof(VoxelizerData), &settings_, gl::BufferStorageMask::DynamicStorageBit);
}
Voxelizer::~Voxelizer() {}

void Voxelizer::setGridMinimum(vec3 const &min) {
	this->settings_.gridMin = min;
	this->updateBuffer();
}

void Voxelizer::setGridMaximum(vec3 const &max) {
	this->settings_.gridMax = max;
	this->updateBuffer();
}

[[nodiscard]] vec3 Voxelizer::getGridMinimum() const {
	return this->settings_.gridMin;
}

[[nodiscard]] vec3 Voxelizer::getGridMaximum() const {
	return this->settings_.gridMax;
}

void Voxelizer::integrityCheck() {
	program_->integrityCheck();
}

void Voxelizer::useProgram() const {
	program_->use();
	program_->setUniform("u_albedoTexture", 0); // Unit = 0
	
	world_grid_->bindImage(0, gl::InternalFormat::Rgba16f, gl::BufferAccessARB::ReadWrite);
	data_buffer_->bindBufferBase(gl::BufferTargetARB::ShaderStorageBuffer, 0);
}

RenderPassInfo const & Voxelizer::renderPassInfo() const {
	static RenderPassInfo info;
	static MaterialBridge bridge;

	info.shader_program = program_.get();
	info.render_sky = false;
	info.bind_model_matrix = true;
	info.bind_albedo_texture = true;
	info.model_matrix_location = programUniform("u_modelMatrix");
	info.viewport = ivec4(0, 0, 512, 512); //< I don't think this matters, we don't use any framebuffer.

	bridge.diffuse_texture_unit = 0;
	bridge.diffuse_texture_is_used = programUniform("u_useAlbedoTexture");
	bridge.diffuse_color_modulation = programUniform("u_baseColorFactor");
	
	info.material_bridge = bridge;
	
	return info;
}

i32 Voxelizer::programUniform(std::string_view const name) const {
	return program_->uniformLocation(std::string(name.data(), name.size()));
}
