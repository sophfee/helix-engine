#include "material.hpp"

#include "util.hpp"
#include "graphics.hpp"

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