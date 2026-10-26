#pragma once
#include "types.hpp"
#include "backends/gpu_resource.hpp"

namespace gfx {
	/**
	 * \brief An image on the GPU. For Vulkan, this helper class wraps both Images and Image Views into one.
	 */
	class Image : public IGpuResource {
	public:
		Image();
		~Image() override;
		
		void free() override;
		void queue_free() override;
		
		[[nodiscard]] bool is_valid() const override;
		
		[[nodiscard]] RID get_subresource(int subresource) const override;
		[[nodiscard]] int get_subresource_count() const override;
		
	private:
		RID image_;
	};
}
