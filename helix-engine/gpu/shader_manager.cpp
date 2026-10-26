#include "shader_manager.hpp"

#pragma comment(lib, "slangd.lib")
#pragma comment(lib, "slang-compiler.lib")

using namespace gfx;

constexpr const char* const search_paths[] = {
	R"(C:/Users/iamqu/Documents/C++ Projects/helix-engine/helix-engine/shaders/)", 
	R"(C:/Users/iamqu/Documents/C++ Projects/helix-engine/helix-engine/shaders/vulkan/)",
	R"(C:/Users/iamqu/Documents/C++ Projects/helix-engine/helix-engine/shaders/global/)",
	R"(C:/Users/iamqu/Documents/C++ Projects/helix-engine/helix-engine/shaders/global/test/)"
};

ShaderManager::ShaderManager() {
	slang::createGlobalSession(global_session_.writeRef());
	
	slang::TargetDesc targetDesc = {};
	targetDesc.format = SLANG_SPIRV;
	targetDesc.profile = global_session_->findProfile("spirv_1_6");
	
	slang::SessionDesc sessionDesc{
		.targets = &targetDesc,
		.targetCount = 1,
		.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
		.searchPaths = search_paths,
		.searchPathCount = 4
	};
	global_session_->createSession(sessionDesc, session_.writeRef());
}

ShaderManager * ShaderManager::get_singleton() {
	static ShaderManager instance;
	return &instance;
}

RID ShaderManager::create_pipeline(const String &path) {
	slang::IBlob* blob;
	slang::IModule* module = session_->loadModule("standard", &blob);
	
	slang::IEntryPoint* entry_point;
	module->getDefinedEntryPoint(0, &entry_point);
	//entry_point->getFunctionReflection()->getParameterByIndex(0)->find
	
	return 0;
}

Slang::ComPtr<slang::IModule> ShaderManager::load_module(const String &name, const String &path) {
	Slang::ComPtr<slang::IBlob> shader;
	Slang::ComPtr<slang::IBlob> diagnostics;
	Slang::ComPtr<slang::IModule> module = Slang::ComPtr(session_->loadModule(name.c_str(), diagnostics.writeRef())); //, path.c_str(), *shader.writeRef(), nullptr));
	if(diagnostics)
	{
		fprintf(stderr, "%s\n", (const char*) diagnostics->getBufferPointer());
	}
	return module;
}
