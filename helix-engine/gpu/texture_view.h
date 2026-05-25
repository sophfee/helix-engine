#pragma once
#include "types.hpp"
#include "engine/disposable.hpp"

class Texture;

class TextureView : public IDisposable {
	u32 original_texture_object_ = 0;
	u32 texture_view_object_ = 0;
	u32 target_ = 0;
	u32 internal_format_ = 0;
	u32 base_level_ = 0;
	u32 num_levels_ = 0;
	u32 base_layer_ = 0;
	u32 num_layers_ = 1;
	bool exists_ = false;

public:
	TextureView(u32 texture_view_object);

	// Copying IS allowed! You will make a duplicate of the texture view, but the underlying texture will be shared. This is because texture views are immutable and can be shared across multiple views.
	TextureView(TextureView const &);
	TextureView(TextureView &&) noexcept;
	TextureView &operator=(TextureView const &);
	TextureView &operator=(TextureView &&) noexcept;
	
	~TextureView() override;
	
	static TextureView from(Texture const &texture);
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
};