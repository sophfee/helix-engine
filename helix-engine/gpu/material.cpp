#include "material.hpp"

#include "util.hpp"
#include "graphics.hpp"

#include <glm/glm.hpp>

#include "driver.hpp"


Material::~Material() {
	GraphicsBackend* dr = GraphicsDriver::get();
	dr->bind_group_delete(bind_group_);
	dr->image_view_delete(diffuse_view_);
	dr->image_view_delete(orm_view_);
	dr->image_view_delete(normal_view_);
	dr->image_view_delete(emissive_view_);
	dr->image_delete(diffuse_);
	dr->image_delete(orm_);
	dr->image_delete(normal_);
	dr->image_delete(emissive_);
}

void Material::draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) {
}

void Material::renderSetup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) {
	if (bind_group_.lower == 0) {
		
		if (!diffuse_view_.valid() || !orm_view_.valid() || !normal_view_.valid()) {
			return;
		}
		
		if (!sampler_.valid()) {
			SamplerDescriptor samplerDescriptor{
				.label = "material_sampler",
				.mag_filter = gfx::Filter::eLinear,
				.min_filter = gfx::Filter::eLinear,
				.mipmap_mode = gfx::MipmapFilter::eLinear,
				.address_mode_u = gfx::AddressMode::eRepeat,
				.address_mode_v = gfx::AddressMode::eRepeat,
				.address_mode_w = gfx::AddressMode::eRepeat,
				.compare_op = gfx::CompareOp::eAlways,
				.enable_anisotropy = true,
				.mip_lod_bias = 0.0f,
				.max_anisotropy = 16.0f,
				.min_lod = 0.0f,
				.max_lod = 1.0f
			};
			
			GraphicsBackend* r = GraphicsDriver::get();
			sampler_ = r->sampler_create(samplerDescriptor);
		}
		
		const BindGroupDescriptor bindGroupDescriptor{
			.label = "material",
			.layout = info.material_bind_group_layout,
			.entries = {
				{
					.binding = 0,
					.resource = BindingResource(
						diffuse_view_,
						gfx::ImageLayout::eShaderReadOnlyOptimal
					)
				}, {
					.binding = 1,
					.resource = BindingResource(
						orm_view_,
						gfx::ImageLayout::eShaderReadOnlyOptimal
					)
				}, {
					.binding = 2,
					.resource = BindingResource(
						normal_view_,
						gfx::ImageLayout::eShaderReadOnlyOptimal
					)
				},{
					.binding = 3,
					.resource = BindingResource(
						sampler_
					)
				}
			}
		};
		
		GraphicsBackend* r = GraphicsDriver::get();
		bind_group_ = r->bind_group_create(bindGroupDescriptor);
	}
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

void Material::setDiffuse(const RID texture, Optional<vec4> const &modulation) {
	diffuse_ = texture;
	if (modulation.has_value())
		diffuse_modulation_ = modulation.value();
	
	ImageViewDescriptor descriptor{
		.label = "material_diffuse_view",
		.image = diffuse_,
		.type = gfx::ImageViewType::e2D,
		.format = gfx::Format::eRgba8Unorm,
		.swizzle = std::nullopt,
		.subresource = ImageSubresourceDescriptor{
			.aspect_mask = gfx::Aspect::eColor
		}
	};
	
	GraphicsBackend* r = GraphicsDriver::get();
	if (diffuse_view_.valid()) r->image_view_delete(diffuse_view_);
	diffuse_view_ = r->image_view_create(descriptor);
}

void Material::setORM(const RID texture) {
	orm_ = texture;
	
	ImageViewDescriptor descriptor{
		.label = "material_orm_view",
		.image = orm_,
		.type = gfx::ImageViewType::e2D,
		.format = gfx::Format::eRgba8Unorm,
		.swizzle = std::nullopt,
		.subresource = ImageSubresourceDescriptor{
			.aspect_mask = gfx::Aspect::eColor
		}
	};
	
	GraphicsBackend* r = GraphicsDriver::get();
	if (orm_view_.valid()) r->image_view_delete(orm_view_);
	orm_view_ = r->image_view_create(descriptor);
}

void Material::setNormal(const RID texture) {
	normal_ = texture;
	
	ImageViewDescriptor descriptor{
		.label = "material_normal_view",
		.image = normal_,
		.type = gfx::ImageViewType::e2D,
		.format = gfx::Format::eRgba8Unorm,
		.swizzle = std::nullopt,
		.subresource = ImageSubresourceDescriptor{
			.aspect_mask = gfx::Aspect::eColor
		}
	};
	
	GraphicsBackend* r = GraphicsDriver::get();
	if (normal_view_.valid()) r->image_view_delete(normal_view_);
	normal_view_ = r->image_view_create(descriptor);
}

void Material::setEmissive(const RID texture) {
	emissive_ = texture;
	
	ImageViewDescriptor descriptor{
		.label = "material_emissive_view",
		.image = emissive_,
		.type = gfx::ImageViewType::e2D,
		.format = gfx::Format::eRgba8Unorm,
		.swizzle = std::nullopt,
		.subresource = ImageSubresourceDescriptor{
			.aspect_mask = gfx::Aspect::eColor
		}
	};
	
	GraphicsBackend* r = GraphicsDriver::get();
	if (emissive_view_.valid()) r->image_view_delete(emissive_view_);
	emissive_view_ = r->image_view_create(descriptor);
}

void Material::bind(RenderPassInfo info) {
}
