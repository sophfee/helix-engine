#pragma once

#include "driver.hpp"
#include "types.hpp"
#include "math.hpp"
#include "util.hpp"
#include "graphics.hpp"
#include "engine/rid.hpp"

class Texture;
class Entity;
class Mesh;
struct RenderPassInfo;

class IMaterial {
public:
	IMaterial() = default;
	virtual ~IMaterial() = default;

	virtual void render_setup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) = 0;
	virtual void draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) = 0;

	virtual void set_shader_parameter(std::string_view const &name, f32 value) = 0;
};

struct GpuMaterial {
	vec4 BaseColorFactor;
	vec3 EmissiveFactor;
    
	int AlphaMode; //< 0 = Opaque, 1 = Mask, 2 = Blend
    
	float RoughnessFactor;
	float MetallicFactor;
	float AlphaCutoff;
    
	float _pad1;
    
	u64 BaseColor;
	u64 MetallicRoughness;
	u64 Normal;
	u64 Emissive;
};

class Material : public IMaterial {
public:
	
	Mutex mutex_;
	
	String name_;
	
	RID diffuse_ = 0;
	RID diffuse_view_ = 0;
	RID orm_ = 0;
	RID orm_view_ = 0;
	RID normal_ = 0;
	RID normal_view_ = 0;
	RID emissive_ = 0;
	RID emissive_view_ = 0;
	RID sampler_ = 0;
	
	RID bind_group_ = 0;

	vec4 diffuse_modulation_ = vec4_one;

	f32 roughness_ = 1.0f;
	f32 roughness_bias_ = 0.0f;

	f32 metallic_ = 0.0f;
	f32 metallic_bias_ = 0.0f;
	
	f32 occlusion_strength_ = 0.0f;
	f32 normal_texture_strength_ = 0.0f;
	
	vec4 emissive_color_mod_ = vec4_one;
	i32 emissive_blend_mode_ = 0;
	f32 emissive_bias_ = 0.0f;
	f32 emissive_scale_ = 0.0f;
	bool dirty_bind_group = false;
bool has_transitioned_all_images = false;
	Material() = default;
	~Material() override;

	void update(RID bind_group_layout);
	void draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) override;
	void render_setup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) override;
	void set_shader_parameter(std::string_view const &name, f32 value) override;
	void set_shader_parameter(std::string_view const &name, vec4 const &value);
	
	GpuMaterial gpu() const;
	
private:
	void create_view(const char* label, RID image, RID &view);
	
public:
	
	void set_diffuse_texture(const RID texture, Optional<vec4> const &modulation);

	[[nodiscard]] RID get_diffuse_texture() const {
		return diffuse_;
	}
	
	[[nodiscard]] RID get_diffuse_texture_view() const {
		return diffuse_view_;
	}

	void set_diffuse_color_modulation(vec4 const &modulation) {
		diffuse_modulation_ = modulation;
	}

	[[nodiscard]] vec4 const &get_diffuse_color_modulation() const {
		return diffuse_modulation_;
	}

	void set_orm_texture(const RID texture);

	[[nodiscard]] RID get_orm_texture() const {
		return orm_;
	}
	
	[[nodiscard]] RID get_orm_texture_view() {
		IGpuDriver* driver = GraphicsSystem::get_driver();
		RID orm_texture = orm_view_;
		if (!orm_view_.valid() && orm_.valid() && driver->is_image_valid(orm_))
			create_view("material_orm_view", orm_, orm_view_);
		else
			orm_texture = driver->get_default_orm_image_view();
		return orm_texture;
	}

	void set_normal_texture(const RID texture);

	[[nodiscard]] RID get_normal_texture() const {
		return normal_;
	}
	
	[[nodiscard]] RID get_normal_texture_view() {
		IGpuDriver* driver = GraphicsSystem::get_driver();
		RID normal_texture = normal_view_;
		if (!normal_view_.valid() && normal_.valid() && driver->is_image_valid(normal_))
			create_view("material_normal_view", normal_, normal_view_);
		else
			normal_texture = driver->get_default_normal_image_view();
		return normal_texture;
	}
	
	void set_emissive_texture(const RID texture);

	[[nodiscard]] RID get_emissive_texture() const {
		return emissive_;
	}

	void bind(RenderPassInfo info);
};