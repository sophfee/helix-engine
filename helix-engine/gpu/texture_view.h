#pragma once
#include "types.hpp"
#include "engine/disposable.hpp"

#if 0
class Texture;
class TextureView : public IDisposable {
	u32 texture_view_object_;
public:
	static TextureView from(Texture const &texture);
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
};
#endif