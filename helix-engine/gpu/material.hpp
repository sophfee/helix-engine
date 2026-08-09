#pragma once

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

	virtual void renderSetup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) = 0;
	virtual void draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) = 0;

	virtual void setShaderParameter(std::string_view const &name, f32 value) = 0;
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

	Material() = default;
	~Material() override;

	void update(RID bind_group_layout);
	void draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) override;
	void renderSetup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) override;
	void setShaderParameter(std::string_view const &name, f32 value) override;
	void setShaderParameter(std::string_view const &name, vec4 const &value);
	
	GpuMaterial gpu() const;
	
private:
	static void createView(const char* label, RID image, RID &view);
	
public:
	
	void setDiffuse(const RID texture, Optional<vec4> const &modulation);

	[[nodiscard]] RID diffuse() const {
		return diffuse_;
	}

	void setDiffuseColorModulation(vec4 const &modulation) {
		diffuse_modulation_ = modulation;
	}

	[[nodiscard]] vec4 const &diffuseColorModulation() const {
		return diffuse_modulation_;
	}

	void setORM(const RID texture);

	[[nodiscard]] RID orm() const {
		return orm_;
	}

	void setNormal(const RID texture);

	[[nodiscard]] RID normal() const {
		return normal_;
	}
	
	void setEmissive(const RID texture);

	[[nodiscard]] RID emissive() const {
		return emissive_;
	}

	void bind(RenderPassInfo info);
};