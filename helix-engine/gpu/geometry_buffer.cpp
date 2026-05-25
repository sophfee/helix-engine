#include "opengl_enums.hpp"
#include "geometry_buffer.hpp"

using enum gl::TextureTarget;

static Texture2DBuilder colorTextureBuilder(std::optional<ivec2> const &screen_resolution = std::nullopt) {
	return Texture2DBuilder()
		.resolution(screen_resolution.value_or(ivec2(1920, 1080)))
		.internalFormat(gl::InternalFormat::Rgba8)
		.pixelFormat(gl::PixelFormat::Rgba)
		.pixelType(gl::PixelType::UnsignedByte)
		.filter(gl::TextureMinFilter::Nearest, gl::TextureMagFilter::Nearest)
		.shouldGenerateMipmaps(false);
}

static Texture2DBuilder normalTextureBuilder(std::optional<ivec2> const &screen_resolution = std::nullopt) {
	return Texture2DBuilder()
		.resolution(screen_resolution.value_or(ivec2(1920, 1080)))
		.internalFormat(gl::InternalFormat::Rgba16f)
		.pixelFormat(gl::PixelFormat::Rgba)
		.pixelType(gl::PixelType::HalfFloat)
		.filter(gl::TextureMinFilter::Nearest, gl::TextureMagFilter::Nearest)
		.shouldGenerateMipmaps(false);
}

static Texture2DBuilder positionTextureBuilder(std::optional<ivec2> const &screen_resolution = std::nullopt) {
	return Texture2DBuilder()
		.resolution(screen_resolution.value_or(ivec2(1920, 1080)))
		.internalFormat(gl::InternalFormat::Rgba32f)
		.pixelFormat(gl::PixelFormat::Rgba)
		.pixelType(gl::PixelType::Float)
		.filter(gl::TextureMinFilter::Nearest, gl::TextureMagFilter::Nearest)
		.shouldGenerateMipmaps(false);
}

static Texture2DBuilder ormTextureBuilder(std::optional<ivec2> const &screen_resolution = std::nullopt) {
	return Texture2DBuilder()
		.resolution(screen_resolution.value_or(ivec2(1920, 1080)))
		.internalFormat(gl::InternalFormat::Rgba8)
		.pixelFormat(gl::PixelFormat::Rgba)
		.pixelType(gl::PixelType::UnsignedByte)
		.filter(gl::TextureMinFilter::Nearest, gl::TextureMagFilter::Nearest)
		.shouldGenerateMipmaps(false);
}

static Texture2DBuilder idTextureBuilder(std::optional<ivec2> const &screen_res = std::nullopt) {
	return Texture2DBuilder()
		.resolution(screen_res.value_or(ivec2(1920, 1080)))
		.internalFormat(gl::InternalFormat::R32ui)
		.pixelFormat(gl::PixelFormat::Red)
		.pixelType(gl::PixelType::UnsignedInt)
		.filter(gl::TextureMinFilter::Nearest, gl::TextureMagFilter::Nearest)
		.shouldGenerateMipmaps(false);
}

static Texture2DBuilder emissiveTextureBuilder(std::optional<ivec2> const &screen_res = std::nullopt) {
	return Texture2DBuilder()
		.resolution(screen_res.value_or(ivec2(1920, 1080)))
		.internalFormat(gl::InternalFormat::Rgba16f)
		.pixelFormat(gl::PixelFormat::Rgba)
		.pixelType(gl::PixelType::HalfFloat)
		.filter(gl::TextureMinFilter::Nearest, gl::TextureMagFilter::Nearest)
		.shouldGenerateMipmaps(false);
}

GBuffer::GBuffer() : storage(nullptr) {
}

GBuffer::GBuffer(ivec2 const &resolution) {
	changeResolution(resolution);
}

GBuffer::~GBuffer() = default;

void GBuffer::changeResolution(ivec2 resolution) {
	using namespace gl;
	storage.reset(new Storage {
		.fb = {},
		.rb = {},
		.color = colorTextureBuilder(resolution),
		.normal = normalTextureBuilder(resolution),
		.position = positionTextureBuilder(resolution),
		.orm = ormTextureBuilder(resolution),
		.id = idTextureBuilder(resolution),
		.emissive = emissiveTextureBuilder(resolution)
	});
	{ //< Nested because of Ambiguous symbol w/ gl::ColorBuffer.
		using enum InternalFormat;
		using enum FramebufferAttachment;

		storage->fb.setLabel("GeoBuffer");
		storage->rb.allocateStorage(resolution, Depth24Stencil8);
		storage->fb.attachRenderbuffer(storage->rb, DepthStencilAttachment);

		storage->color.setLabel("GBuffer: Albedo");
		storage->normal.setLabel("GBuffer: Normal");
		storage->position.setLabel("GBuffer: Position");
		storage->orm.setLabel("GBuffer: ORM");
		storage->id.setLabel("GBuffer: ID");
		storage->emissive.setLabel("GBuffer: Emissive");

		storage->fb.attachTexture(ColorAttachment0, storage->color, 0);
		storage->fb.attachTexture(ColorAttachment1, storage->normal, 0);
		storage->fb.attachTexture(ColorAttachment2, storage->position, 0);
		storage->fb.attachTexture(ColorAttachment3, storage->orm, 0);
		storage->fb.attachTexture(ColorAttachment4, storage->id, 0);
		storage->fb.attachTexture(ColorAttachment5, storage->emissive, 0);
	}
	
	using enum ColorBuffer;
	storage->fb.setDrawBuffers({
		ColorAttachment0,
		ColorAttachment1,
		ColorAttachment2,
		ColorAttachment3,
		ColorAttachment4,
		ColorAttachment5
	});

	FramebufferStatus const status = storage->fb.status();
	if (status != FramebufferStatus::FramebufferComplete)
		assert(false && "Incomplete framebuffer!");
}
void GBuffer::bind() const {
	storage->fb.bind();
}
void GBuffer::unbind() const {
	storage->fb.unbind();
}
void GBuffer::bindForDrawing() const {
	using enum gl::FramebufferTarget;
	storage->fb.bind(DrawFramebuffer);
}
void GBuffer::unbindForDrawing() const {
	using enum gl::FramebufferTarget;
	storage->fb.unbind(DrawFramebuffer);
}
void GBuffer::bindForReading() const {
	using enum gl::FramebufferTarget;
	storage->fb.bind(ReadFramebuffer);
}
void GBuffer::unbindForReading() const {
	using enum gl::FramebufferTarget;
	storage->fb.unbind(ReadFramebuffer);
}

Texture const & GBuffer::color() const {
	return storage->color;
}
Texture const & GBuffer::normal() const {
	return storage->normal;
}
Texture const & GBuffer::position() const {
	return storage->position;
}
Texture const & GBuffer::orm() const {
	return storage->orm;
}
Texture const & GBuffer::id() const {
	return storage->id;
}
Texture const & GBuffer::emissive() const {
	return storage->emissive;
}