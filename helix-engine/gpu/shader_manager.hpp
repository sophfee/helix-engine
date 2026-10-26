#pragma once

#include <slang/slang-com-ptr.h>
#include <slang/slang.h>

#include "types.hpp"
#include "engine/rid.hpp"

namespace gfx {
	class ShaderManager {
	public:
		static ShaderManager* get_singleton();
		
		RID create_pipeline(const String &path);
		Slang::ComPtr<slang::IModule> load_module(const String &name, const String &path);
		
	private:
		ShaderManager();
		Slang::ComPtr<slang::IGlobalSession> global_session_;
		Slang::ComPtr<slang::ISession> session_;
		
	};
}
