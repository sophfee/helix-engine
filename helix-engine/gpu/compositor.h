#pragma once
#include "graphics.hpp"
#include "texture.h"
#include "types.hpp"

class Compositor : public IDisposable {
public:
	Compositor();

	void bindForWriting() const;
	void clear() const;
	void bindRenderOutputToUnit(u32 const unit) const;
	void resize(ivec2 const &new_size);
	void tonemap(ivec2 const &screenResolution, Texture const &sample, f32 exposure);

private:

	void resizeIfNeeded(ivec2 const &new_size);
	static Texture2DBuilder raw3DOutputBuilder(ivec2 const &resolution = ivec2(1920, 1080));

public:
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

private:
	Program		tonemapper;

	struct CompositorStorage {
		Texture		output;
		Framebuffer framebuffer;
	};

	ivec2 resolution;
	Box<CompositorStorage> storage;
};
