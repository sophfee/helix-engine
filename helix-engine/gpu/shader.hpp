#pragma once
#include "backends/gpu_resource.hpp"
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>
#include <slang/slang-com-helper.h>

namespace slang {
	struct IModule;
}

namespace gfx {
	class Shader : public IGpuResource {
	public:
		Shader(const String &name, const String &path);
		
		void free() override;
		void queue_free() override;
		[[nodiscard]] bool is_valid() const override;
		[[nodiscard]] RID get_subresource(int subresource) const override;
		[[nodiscard]] int get_subresource_count() const override;
		
	protected:
		void reflect();
		void process_engine_inputs();
		
	private:
		Slang::ComPtr<slang::IModule> module_ = nullptr;
	};
}
