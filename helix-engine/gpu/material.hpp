#pragma once

#include "types.hpp"
#include "math.hpp"
#include "util.hpp"
#include "graphics.hpp"

namespace helix::render {
	class Texture;
	class Entity;
	class Mesh;
	struct RenderPassInfo;

	class ShaderProvider final {
		static SharedPtr<ShaderProvider> instance_;
	public:

		static Result<Program &> requestShaderProgram(std::string_view const &path);
	
	private:
		Vec<u32> shaderHashVec_;
		Vec<Program> shaderProgramVec_;
	};

	class IMaterial {
	public:
		IMaterial() = default;
		virtual ~IMaterial() = default;

		virtual void renderSetup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) = 0;
		virtual void draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) = 0;

		virtual void setShaderParameter(std::string_view const &name, f32 value) = 0;
	};


	class Material : public IMaterial {
	public:
		SharedPtr<Texture> diffuse_;
		SharedPtr<Texture> orm_;
		SharedPtr<Texture> normal_;
		SharedPtr<Texture> emissive_;

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
		~Material() override = default;

		void draw(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) override;
		void renderSetup(RenderPassInfo const &info, Mesh const &mesh, Entity const &entity) override;
		void setShaderParameter(std::string_view const &name, f32 value) override;

		void bind(RenderPassInfo const &info) const;
	
		void setDiffuse(SharedPtr<Texture> const &texture, Optional<vec4> const &modulation) {
			diffuse_ = texture;
			if (modulation.has_value())
				diffuse_modulation_ = modulation.value();
		}

		[[nodiscard]] SharedPtr<Texture> const &diffuse() const {
			return diffuse_;
		}

		void setDiffuseColorModulation(vec4 const &modulation) {
			diffuse_modulation_ = modulation;
		}

		[[nodiscard]] vec4 const &diffuseColorModulation() const {
			return diffuse_modulation_;
		}

		void setORM(SharedPtr<Texture> const &texture) {
			orm_ = texture;
		}
	
		[[nodiscard]] SharedPtr<Texture> const &orm() const {
			return orm_;
		}

		void setNormal(SharedPtr<Texture> const &texture) {
			normal_ = texture;
		}
	
		[[nodiscard]] SharedPtr<Texture> const &normal() const {
			return normal_;
		}
	
		void setEmissive(SharedPtr<Texture> const &texture) {
			emissive_ = texture;
		}
	
		[[nodiscard]] SharedPtr<Texture> const &emissive() const {
			return emissive_;
		}
	};
}