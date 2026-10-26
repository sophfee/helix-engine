#include "shader.hpp"

#include <iostream>

#include "shader_manager.hpp"

using namespace gfx;

Shader::Shader(const String &name, const String &path) {
	ShaderManager* shader_manager = ShaderManager::get_singleton();
	this->module_ = shader_manager->load_module(name, path);
	assert(module_.get() != nullptr);
	reflect();
}

void Shader::free() {
	
}

void Shader::queue_free() {
}

bool Shader::is_valid() const {
	return true;
}

RID Shader::get_subresource(int subresource) const {
	return 0;
}

int Shader::get_subresource_count() const {
	return 0;
}

void Shader::reflect() {
	slang::DeclReflection *reflection = module_->getModuleReflection();
	for (slang::DeclReflection *child : reflection->getChildren()) {
		//if (child && child->getName())
		//	std::cout << child->getName() << '\n';
		
		if (child->getType()->getKind() == slang::TypeReflection::Kind::Struct) {
		}
	}
}

void Shader::process_engine_inputs() {
}
