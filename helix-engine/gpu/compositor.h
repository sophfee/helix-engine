#pragma once
#include "framebuffer.h"
#include "graphics.hpp"
#include "texture.h"
#include "types.hpp"

class Compositor : public IDisposable {
public:
	Compositor();

	void beginDraw() const;
	void endDraw() const;
	
	void clear() const;
	void bindRenderOutputToUnit(u32 const unit) const;
	void resize(ivec2 const &new_size);
	void tonemap(ivec2 const &screenResolution, Texture const &sample, f32 exposure);

	void integrityCheck();

	Texture const &ssrTexture() const;

private:

	void createCompositeStorage(ivec2 const &size);
	void resizeIfNeeded(ivec2 const &new_size);

public:
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

private:
	Program		tonemapper;

	struct CompositorStorage {
		Texture	compositeTexture;
		Texture ssrTexture;
		Framebuffer compositeFramebuffer;
	};

	ivec2 resolution;
	Box<CompositorStorage> storage;
};
