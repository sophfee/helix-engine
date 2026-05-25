#include "compositor.h"
#include "compositor.h"

#include "framebuffer.h"
#include "gltf.h"

static Texture2DBuilder compositeBuilder(ivec2 const &resolution) {
	return Texture2DBuilder()
		.internalFormat(gl::InternalFormat::Rgba8)
		.pixelFormat(gl::PixelFormat::Rgba)
		.pixelType(gl::PixelType::UnsignedByte)
		.wrapMode(gl::TextureWrapMode::ClampToEdge)
		.resolution(resolution);
}

// SSR results
static Texture2DBuilder ssrTextureBuilder(ivec2 const &resolution) {
	return Texture2DBuilder()
	.internalFormat(gl::InternalFormat::Rgba8)
	.pixelFormat(gl::PixelFormat::Rgba)
	.pixelType(gl::PixelType::UnsignedByte)
	.wrapMode(gl::TextureWrapMode::ClampToEdge)
	.resolution(resolution / 2);
}

Compositor::Compositor()
	: IDisposable(), tonemapper("shaders\\compositor\\tonemap.comp") {
	resize(ivec2{1920, 1080});
}
void Compositor::beginDraw() const {
	storage->compositeFramebuffer.bind(gl::FramebufferTarget::Framebuffer);
}
void Compositor::endDraw() const {
	storage->compositeFramebuffer.unbind(gl::FramebufferTarget::Framebuffer);
}

void Compositor::clear() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Compositor::bindRenderOutputToUnit(u32 const unit) const {
	storage->compositeTexture.bindTextureUnit(unit);
}

void Compositor::resize(ivec2 const &new_size) {
	if (new_size == resolution)
		return;

	resolution = new_size;
	createCompositeStorage(new_size);
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
void Compositor::integrityCheck() {
	tonemapper.integrityCheck();
}

Texture const & Compositor::ssrTexture() const {
	return storage->ssrTexture;
}

void Compositor::createCompositeStorage(ivec2 const &size) {
	storage.reset(new CompositorStorage{
		.compositeTexture = compositeBuilder(size),
		.ssrTexture = ssrTextureBuilder(size),
		.compositeFramebuffer = {}
	});

	{
		using enum gl::FramebufferAttachment;
		storage->compositeFramebuffer.attachTextures({
			{ ColorAttachment0, std::ref(storage->compositeTexture) }
		});
	}

	using enum gl::ColorBuffer;
	storage->compositeFramebuffer.setDrawBuffers({ ColorAttachment0 });

	using enum gl::FramebufferStatus;
	assert(storage->compositeFramebuffer.status() == FramebufferComplete);
}

void Compositor::resizeIfNeeded(ivec2 const &new_size) {
	if (resolution == new_size)
		return;
	resize(new_size);
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
