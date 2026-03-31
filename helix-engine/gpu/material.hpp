#pragma once
#include "types.hpp"

class Entity;
class Mesh;
class Program;
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