#include "material.hpp"

#include "util.hpp"
#include "graphics.hpp"

#include <glm/glm.hpp>

#include "driver.hpp"


Material::~Material() {
	GraphicsBackend *dr = GraphicsDriver::get();
	dr->destroy_bind_group(bind_group_);
	dr->destroy_image_view(diffuse_view_);
	dr->destroy_image_view(orm_view_);
	dr->destroy_image_view(normal_view_);
	dr->destroy_image_view(emissive_view_);
	dr->destroy_image(diffuse_);
	dr->destroy_image(orm_);
	dr->destroy_image(normal_);
	dr->destroy_image(emissive_);
	dr->destroy_sampler(sampler_);
}

void Material::update(const RID bind_group_layout) {
	GraphicsBackend *driver = GraphicsDriver::get();
	
	if (!diffuse_view_.valid() && diffuse_.valid() && driver->is_image_valid(diffuse_)) {
		create_view("material_diffuse_view", diffuse_, diffuse_view_);
	}
	if (!orm_view_.valid() && orm_.valid() && driver->is_image_valid(orm_)) {
		create_view("material_orm_view", orm_, orm_view_);
	}
	if (!normal_view_.valid() && normal_.valid() && driver->is_image_valid(normal_)) {
		create_view("material_normal_view", normal_, normal_view_);
	}
	
	if (bind_group_.lower == 0) {
		if (!diffuse_view_.valid() || !orm_view_.valid() || !normal_view_.valid()) {
			return;
		}

		if (!sampler_.valid()) {
			const SamplerDescriptor samplerDescriptor{
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

			sampler_ = driver->create_sampler(samplerDescriptor);
		}

		const BindGroupDescriptor bindGroupDescriptor{
			.label = "material",
			.layout = bind_group_layout,
			.entries = {
				{
					.binding = 0,
					.resource = BindingResource(
						diffuse_view_,
						gfx::ImageLayout::eReadOnly
					)
				},
				{
					.binding = 1,
					.resource = BindingResource(
						orm_view_,
						gfx::ImageLayout::eReadOnly
					)
				},
				{
					.binding = 2,
					.resource = BindingResource(
						normal_view_,
						gfx::ImageLayout::eReadOnly
					)
				},
				{
					.binding = 3,
					.resource = BindingResource(
						sampler_
					)
				}
			}
		};
		
		assert(driver->is_image_view_valid(diffuse_view_));
		assert(driver->is_image_view_valid(orm_view_));
		assert(driver->is_image_view_valid(normal_view_));

		bind_group_ = driver->create_bind_group(bindGroupDescriptor);
			
		printf("Material bind group created: %u\n", bind_group_.upper);
	}
}

void Material::draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) {
	
}

void Material::render_setup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) {
}

void Material::set_shader_parameter(std::string_view const &name, const f32 value) {
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

void Material::set_shader_parameter(std::string_view const &name, vec4 const &value) {
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
		.BaseColor = 0,         //diffuse_ ? diffuse_->textureHandle() : 0,
		.MetallicRoughness = 0, //orm_ ? orm_->textureHandle() : 0,
		.Normal = 0,            //normal_ ? normal_->textureHandle() : 0,
		.Emissive = 0           //emissive_ ? emissive_->textureHandle() : 0,
	};
}

void Material::create_view(const char *label, const RID image, RID &view) {
	GraphicsBackend *r = GraphicsDriver::get();
	if (!r->is_image_valid(image)) return;
	const ImageViewDescriptor descriptor{
		.label = label,
		.image = image,
		.type = gfx::ImageViewType::e2D,
		.format = gfx::Format::eBc1RgbaUnormBlock,
		.swizzle = std::nullopt,
		.usage = gfx::ImageUsage::eSampled,
		.subresource = ImageSubresourceDescriptor{
			.aspect_mask = gfx::Aspect::eColor
		}
	};
	if (view.valid()) r->destroy_image_view(view);
	view = r->create_image_view(descriptor);
}

void Material::set_diffuse_texture(const RID texture, Optional<vec4> const &modulation) {
	diffuse_ = texture;
	if (modulation.has_value())
		diffuse_modulation_ = modulation.value();

}

void Material::set_orm_texture(const RID texture) {
	orm_ = texture;
}

void Material::set_normal_texture(const RID texture) {
	normal_ = texture;
}

void Material::set_emissive_texture(const RID texture) {
	emissive_ = texture;
}

void Material::bind(RenderPassInfo info) {
}
