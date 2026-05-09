#include "compositor.h"
#include "compositor.h"

#include "framebuffer.h"

Compositor::Compositor()
	: IDisposable(), tonemapper("shaders\\compositor\\tonemapper.comp") {
	resize(ivec2{1920, 1080});
}

void Compositor::bindForWriting() const {
	storage->compositeFramebuffer.bind();
}
void Compositor::clear() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Compositor::bindRenderOutputToUnit(u32 const unit) const {
	storage->compositeTexture.bindTextureUnit(unit);
}

void Compositor::resize(ivec2 const &new_size) {
	
	storage.reset(
		new CompositorStorage{ raw3DOutputBuilder(new_size), {} }
	);
	storage->compositeFramebuffer.attachTexture(gl::FramebufferAttachment::ColorAttachment0, storage->compositeTexture);
	storage->compositeFramebuffer.setDrawBuffers({ gl::ColorBuffer::ColorAttachment0 });
	storage->compositeFramebuffer.setReadBuffer({ });
	assert(storage->compositeFramebuffer.status() == gl::FramebufferStatus::FramebufferComplete);
}

void Compositor::tonemap(ivec2 const &screenResolution, Texture const &sample, f32 exposure) {
	resizeIfNeeded(screenResolution);
	tonemapper.use();

	storage->compositeTexture.bindImage(0, gl::InternalFormat::Rgba16f, gl::BufferAccessARB::WriteOnly);
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

void Compositor::createGeometryBuffer(ivec2 const &resolution) {
	Texture2DBuilder builder = Texture2DBuilder()
		.internalFormat(gl::InternalFormat::Rgba16f)
		.pixelFormat(gl::PixelFormat::Rgba)
		.pixelType(gl::PixelType::Float)
		.wrapMode(gl::TextureWrapMode::ClampToEdge)
		.resolution(resolution);
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

	if (!storage->compositeTexture.disposed())
		 storage->compositeTexture.dispose();

	if (!storage->compositeFramebuffer.disposed())
		 storage->compositeFramebuffer.dispose();

	storage.reset();
}

bool Compositor::disposed() const {
	return storage == nullptr;
}
