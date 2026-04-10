#include "compositor.h"
#include "compositor.h"

Compositor::Compositor()
	: IDisposable(), tonemapper("shaders\\compositor\\tonemapper.comp") {
}

void Compositor::bindForWriting() const {
	storage->framebuffer.bind();
}
void Compositor::clear() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Compositor::bindRenderOutputToUnit(u32 const unit) const {
	storage->output.bindTextureUnit(unit);
}

void Compositor::resize(ivec2 const &new_size) {
	storage.reset(
		new CompositorStorage{ raw3DOutputBuilder(new_size), {} }
	);
	storage->framebuffer.attachTexture(gl::FramebufferAttachment::ColorAttachment0, storage->output);
	storage->framebuffer.setDrawBuffers({ gl::ColorBuffer::ColorAttachment0 });
	storage->framebuffer.setReadBuffer({ });
	assert(storage->framebuffer.status() == gl::FramebufferStatus::FramebufferComplete);
}
void Compositor::tonemap(ivec2 const &screenResolution, Texture const &sample, f32 exposure) {
	resizeIfNeeded(screenResolution);
	tonemapper.use();

	storage->output.bindImage(0, gl::InternalFormat::Rgba16f, gl::BufferAccessARB::WriteOnly);
	sample.bindImage(1, gl::InternalFormat::Rgba16f, gl::BufferAccessARB::ReadOnly);

	tonemapper.setUniform(0, 0); // Sample Texture
	tonemapper.setUniform(1, 1);
	
	tonemapper.setUniform(2, exposure); // Target Exposure

	ivec2 const workGroups(
		(screenResolution.x + 7) / 8,
		(screenResolution.y + 7) / 8
	);
	glDispatchCompute(workGroups.x, workGroups.y, 1);
}

void Compositor::resizeIfNeeded(ivec2 const &new_size) {
	if (resolution == new_size)
		return;
	resize(new_size);
}

Texture2DBuilder Compositor::raw3DOutputBuilder(ivec2 const &resolution) {
	return Texture2DBuilder()
		.internalFormat(gl::InternalFormat::Rgba16f)
		.pixelFormat(gl::PixelFormat::Rgba)
		.pixelType(gl::PixelType::Float)
		.wrapMode(gl::TextureWrapMode::ClampToEdge)
		.resolution(resolution);
}

void Compositor::dispose() {
	if (storage == nullptr)
		return;

	if (!storage->output.disposed())
		 storage->output.dispose();

	if (!storage->framebuffer.disposed())
		 storage->framebuffer.dispose();

	storage.reset();
}

bool Compositor::disposed() const {
	return storage == nullptr;
}
