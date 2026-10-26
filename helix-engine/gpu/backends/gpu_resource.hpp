#pragma once
#include "types.hpp"

namespace gfx {
	class IGpuResource {
	public:
		virtual ~IGpuResource() = default;
		virtual void free() = 0;
		virtual void queue_free() = 0;
		
		[[nodiscard]] virtual bool is_valid() const = 0;
		
		/// Subresource identifies any additional RIDs associated with the object, for example, an Image will have an integrated Image View, the Image View RID can be retrieved by calling get_rid(1), while the image itself is get_rid(0)
		[[nodiscard]] virtual RID get_subresource(int subresource = 0) const = 0;
		[[nodiscard]] virtual int get_subresource_count() const = 0;
	};
}
