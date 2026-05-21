#include "material.hpp"

#include "util.hpp"
#include "graphics.hpp"
#include "texture.h"

SharedPtr<ShaderProvider> ShaderProvider::instance_ = nullptr; // allocate in main;

Result<Program &> ShaderProvider::requestShaderProgram(std::string_view const &path) {
	HELIX_ASSUME(instance_ != nullptr, "ShaderProvider::instance_ must be initialized before requesting shader programs.");
	for (std::size_t i = 0; i < instance_->shaderHashVec_.size(); ++i) {
		u32 hashValue = instance_->shaderHashVec_[i];
		if (hash(path) == hashValue)
			return (instance_->shaderProgramVec_[i]);
	}
	return instance_->shaderProgramVec_[0];
}

void Material::draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) {
}
void Material::renderSetup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) {
	bind(info);
}
void Material::setShaderParameter(std::string_view const &name, f32 value) {}

void Material::bind(RenderPassInfo const &info) const {
	MaterialBridge const &bridge = info.material_bridge;
	Program const *program = info.shader_program;
	
	bool const uses_diffuse_texture = bridge.diffuse_texture_unit != -1;
	bool const uses_is_diffuse_texture_used = bridge.diffuse_texture_is_used != -1;
	bool const uses_diffuse_color_modulation = bridge.diffuse_color_modulation != -1;
	
	if (diffuse_ != nullptr) {
		if (uses_diffuse_texture) diffuse_->bindTextureUnit(bridge.diffuse_texture_unit);
		if (uses_is_diffuse_texture_used) program->setUniform(bridge.diffuse_texture_is_used, true);
	}
	else if (uses_is_diffuse_texture_used)
		program->setUniform(bridge.diffuse_texture_is_used, false);

	if (uses_diffuse_color_modulation)
		program->setUniform(bridge.diffuse_color_modulation, diffuse_modulation_);

	// ORM
	bool const uses_orm_texture = bridge.orm_texture_unit != -1;
	bool const uses_orm_texture_is_used = bridge.orm_texture_is_used != -1;
	bool const uses_roughness_scale = bridge.roughness_scale != -1;
	bool const uses_roughness_bias = bridge.roughness_bias != -1;
	bool const uses_metallic_scale = bridge.metallic_scale != -1;
	bool const uses_metallic_bias = bridge.metallic_bias != -1;
	
	if (orm_ != nullptr) {
		if (uses_orm_texture) orm_->bindTextureUnit(bridge.orm_texture_unit);
		if (uses_orm_texture_is_used) program->setUniform(bridge.orm_texture_is_used, 1);
	}
	else if (uses_orm_texture_is_used)
		program->setUniform(bridge.orm_texture_is_used, 0);

	if (uses_roughness_scale) program->setUniform(bridge.roughness_scale, roughness_);
	if (uses_roughness_bias)  program->setUniform(bridge.roughness_bias, roughness_bias_);

	if (uses_metallic_scale) program->setUniform(bridge.metallic_scale, metallic_);
	if (uses_metallic_bias)  program->setUniform(bridge.metallic_bias, metallic_bias_);

	// normal
	bool const uses_normal_texture = bridge.normal_texture_unit != -1;
	bool const uses_normal_texture_is_used = bridge.normal_texture_is_used != -1;
	bool const uses_normal_texture_strength = bridge.normal_texture_strength != -1;
	
	if (normal_ != nullptr) {
		if (uses_normal_texture) normal_->bindTextureUnit(bridge.normal_texture_unit);
		if (uses_normal_texture_is_used)  program->setUniform(bridge.normal_texture_is_used, 1);
		if (uses_normal_texture_strength) program->setUniform(bridge.normal_texture_strength, normal_texture_strength_);
	}
	else if (uses_normal_texture_is_used)
		program->setUniform(bridge.normal_texture_is_used, 0);

	// emissive
	bool const uses_emissive_texture = bridge.emissive_texture_unit != -1;
	bool const uses_emissive_texture_is_used = bridge.emissive_texture_is_used != -1;
	bool const uses_emissive_color_modulation = bridge.emissive_color_modulation != -1;
	bool const uses_emissive_blend_mode = bridge.emissive_blend_mode != -1;
	bool const uses_emissive_bias = bridge.emissive_bias != -1;
	bool const uses_emissive_scale = bridge.emissive_scale != -1;
	
	if (emissive_ != nullptr) {
		if (uses_emissive_texture)
			emissive_->bindTextureUnit(bridge.emissive_texture_unit);
		if (uses_emissive_texture_is_used)
			program->setUniform(bridge.emissive_texture_is_used, 1);
		if (uses_emissive_color_modulation)
			program->setUniform(bridge.emissive_color_modulation, emissive_color_mod_);
		if (uses_emissive_blend_mode)
			program->setUniform(bridge.emissive_blend_mode, emissive_blend_mode_);
		if (uses_emissive_bias)
			program->setUniform(bridge.emissive_bias, emissive_bias_);
		if (uses_emissive_scale)
			program->setUniform(bridge.emissive_scale, emissive_scale_);
	}
	else if (uses_emissive_texture_is_used)
		program->setUniform(bridge.emissive_texture_is_used, 0);
}
