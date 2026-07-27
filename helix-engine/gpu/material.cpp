#include "material.hpp"

#include "util.hpp"
#include "graphics.hpp"

#include <glm/glm.hpp>


void Material::draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) {
}
void Material::renderSetup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) {
}

void Material::setShaderParameter(std::string_view const &name, f32 value) {
	switch (hash(name)) {
		case hash("roughness"):
			roughness_ = value;
			break;
		case hash("roughness_bias"):
			roughness_bias_ = value;
			break;
		case hash("metallic"):
			metallic_ = value;
			break;
		case hash("metallic_bias"):
			metallic_bias_ = value;
			break;
		case hash("occlusion_strength"):
			occlusion_strength_ = value;
			break;
		case hash("normal_texture_strength"):
			normal_texture_strength_ = value;
			break;
		case hash("emissive_blend_mode"):
			emissive_blend_mode_ = static_cast<i32>(value);
			break;
		case hash("emissive_bias"):
			emissive_bias_ = value;
			break;
		case hash("emissive_scale"):
			emissive_scale_ = value;
			break;
		default:
			return;
	}
}

void Material::setShaderParameter(std::string_view const &name, vec4 const &value) {
	switch (hash(name)) {
		case hash("diffuse_color_modulation"):
			diffuse_modulation_ = value;
			break;
		case hash("emissive_color_modulation"):
			emissive_color_mod_ = value;
			break;
		default:
			return;
	}
}

GpuMaterial Material::gpu() const {
	return {
		.BaseColorFactor = diffuse_modulation_,
		.EmissiveFactor = xyz(emissive_color_mod_) * emissive_color_mod_.a,
		.AlphaMode = 0,
		.RoughnessFactor = roughness_,
		.MetallicFactor = metallic_,
		.AlphaCutoff = 0.5f,
		.BaseColor = 0,//diffuse_ ? diffuse_->textureHandle() : 0,
		.MetallicRoughness = 0,//orm_ ? orm_->textureHandle() : 0,
		.Normal = 0,//normal_ ? normal_->textureHandle() : 0,
		.Emissive = 0 //emissive_ ? emissive_->textureHandle() : 0,
	};
}

void Material::bind(RenderPassInfo info) {
}
