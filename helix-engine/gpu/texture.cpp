#include "texture.h"

#include <glm/gtc/type_ptr.hpp>

#include "graphics.hpp"
#include "imgui.h"
#include "opengl_enums2.hpp"
#include "render_server.h"
#include "glad/glad.h"

// Texture

u32 Texture::bound_texture_2d_ = 0xFFFFFFFFu;

AsyncTextureBank::AsyncTextureBank() : sem_(32), buffers_registered_(32) {
	for (auto &[in_use, memsize, buffer] : buffers_registered_) {
		in_use = false;
		memsize = 4096ull * 4096ull * 4ull;
		buffer = std::make_shared<Buffer>();
		buffer->allocStorage(4096ull * 4096ull * 4ull, nullptr, gl::BufferStorageMask::ClientStorageBit | gl::BufferStorageMask::MapPersistentBit | gl::BufferStorageMask::MapWriteBit);
		printf("Buffer object %u\n", buffer->buffer_object_);
	}
}
void Texture::createObject(gl::TextureTarget target) {
	glCreateTextures((GLenum)target, 1, &texture_object_);
}

Texture::Texture(gl::TextureTarget p_textureTarget) : internal_format_(gl::InternalFormat::Rgb8), pixel_format_(gl::PixelFormat::Rgb), pixel_type_(gl::PixelType::UnsignedByte) {
	glCreateTextures(static_cast<GLenum>(p_textureTarget), 1, &texture_object_);
	RenderServer::singleton().track(this);
}

Texture::Texture(u32 const existing_texture_object_) : texture_object_(existing_texture_object_) {
	i32 internalFormat, pixelFormat, pixelType;
	glGetTextureParameteriv(texture_object_, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
	glGetTextureParameteriv(texture_object_, GL_TEXTURE_INTERNAL_FORMAT, &pixelFormat);
	glGetTextureParameteriv(texture_object_, GL_TEXTURE_INTERNAL_FORMAT, &pixelType);
	this->internal_format_ = static_cast<gl::InternalFormat>(internalFormat);
	this->pixel_format_ = static_cast<gl::PixelFormat>(pixelFormat);
	this->pixel_type_ = static_cast<gl::PixelType>(pixelType);

	glGetTextureParameteriv(texture_object_, GL_TEXTURE_WIDTH, &resolution_.x);
	glGetTextureParameteriv(texture_object_, GL_TEXTURE_WIDTH, &resolution_.y);
	glGetTextureParameteriv(texture_object_, GL_TEXTURE_DEPTH, &layers_);
	glGetTextureParameteriv(texture_object_, GL_TEXTURE_MAG_FILTER, (int*)&this->mag_filter_);
	glGetTextureParameteriv(texture_object_, GL_TEXTURE_MIN_FILTER, (int*)&this->min_filter_);
	RenderServer::singleton().track(this);
}

Texture::~Texture() {
	glDeleteTextures(1, &texture_object_);
}

void Texture::bind(gl::TextureTarget target) const {
	if (!is_dsa_) {
		glBindTexture(static_cast<GLenum>(target), texture_object_);
	}
}

void Texture::setLabel(_STD string_view const name) const {
	glObjectLabel(GL_TEXTURE, texture_object_, static_cast<GLsizei>(name.size()), name.data());
}

i32 Texture::paramInt(gl::GetTextureParameter parameter) const {
	if (is_dsa_) {
		i32 iValue;
		glGetTextureParameteriv(texture_object_, static_cast<GLenum>(parameter), &iValue);
		return iValue;
	}
	else {
		bind(gl::TextureTarget::Texture2D);
		i32 iValue;
		glTexParameteriv(GL_TEXTURE_2D, static_cast<GLenum>(parameter), &iValue);
		return iValue;
	}
}

void Texture::setParamInt(gl::GetTextureParameter parameter, i32 const value) const {
	if (is_dsa_) {
		glTextureParameteri(texture_object_, static_cast<GLenum>(parameter), value); gpu_check;
	}
	else {
		bind(gl::TextureTarget::Texture2D);
		glTexParameteri(GL_TEXTURE_2D, static_cast<GLenum>(parameter), value); gpu_check;
	}
}

i32 Texture::paramIntLevel(gl::GetTextureParameter parameter, i32 const level) const {
	i32 iValue;
	glGetTextureLevelParameteriv(texture_object_, level, static_cast<GLenum>(parameter), &iValue);
	return iValue;
}

u32 Texture::paramUInt(gl::GetTextureParameter parameter) const {
	u32 uiValue;
	glGetTextureParameterIuiv(texture_object_, static_cast<GLenum>(parameter), &uiValue);
	return uiValue;
}

u32 Texture::paramUIntLevel(gl::GetTextureParameter parameter, i32 level) const {
	u32 uiValue;
	glGetTextureParameterIuiv(texture_object_, level, &uiValue);
	return uiValue;
}

void Texture::setParamUInt(gl::GetTextureParameter parameter, u32 const value) const {
	glTextureParameterIuiv(texture_object_, static_cast<GLenum>(parameter), &value); gpu_check;
}

_STD vector<i32> Texture::intVecParam(gl::GetTextureParameter parameter) const {
	return {};
}

void Texture::setIntVecParam(gl::GetTextureParameter parameter, _STD vector<i32> const &value) const {
}

void Texture::setWrapMode(gl::TextureWrapMode wrap_mode, std::optional<gl::TextureWrapMode> const wrap_s, std::optional<gl::TextureWrapMode> const wrap_t) const {
	setParamI<gl::GetTextureParameter::TextureWrapS>(wrap_s.value_or(wrap_mode));
	setParamI<gl::GetTextureParameter::TextureWrapT>(wrap_t.value_or(wrap_mode));
}

void Texture::setFilter(gl::TextureMinFilter const min_filter, gl::TextureMagFilter const mag_filter) const {
	setParamI<gl::GetTextureParameter::TextureMinFilter>(min_filter);
	setParamI<gl::GetTextureParameter::TextureMagFilter>(mag_filter);
}

void Texture::setBorderColor(vec4 const &color) const {
	glTextureParameterfv(texture_object_, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color)); gpu_check;
}

void Texture::setCompareMode(gl::TextureCompareMode compare_mode) const {
	glTextureParameteri(texture_object_, GL_TEXTURE_COMPARE_MODE, static_cast<GLenum>(compare_mode)); gpu_check;
}
void Texture::setCompareFunction(gl::CompareFunction compare_function) const {
	glTextureParameteri(texture_object_, GL_TEXTURE_COMPARE_FUNC, static_cast<GLenum>(compare_function)); gpu_check;
}

vec4 Texture::borderColor() const {
	vec4 color(0.0f);
	glGetTextureParameterfv(texture_object_, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color)); gpu_check;
	return color;
}

void Texture::generateMipmap() const {
	glGenerateTextureMipmap(texture_object_); gpu_check;
}

void Texture::setAnisotropicFilteringEnabled(bool const enabled) {
	glTextureParameterf(texture_object_, GL_TEXTURE_MAX_ANISOTROPY, enabled ? 16.0f : 1.0f); gpu_check;
	anisotropic_filtering_enabled_ = enabled;
}

void Texture::enableAnisotropicFiltering() {
	setAnisotropicFilteringEnabled(true);
}

void Texture::disableAnisotropicFiltering() {
	setAnisotropicFilteringEnabled(false);
}

bool Texture::isAnisotropicFilteringEnabled() const {
	return anisotropic_filtering_enabled_;
}
void Texture::bindImage(gl::uint32_t const unit, gl::InternalFormat format, gl::BufferAccessARB access, gl::int32_t const level, bool const layered, gl::int32_t const layer) const {
	glBindImageTexture(
		unit,
		texture_object_,
		level,
		layered,
		layer,
		static_cast<GLenum>(access),
		static_cast<GLenum>(format)
	); gpu_check;
}

void Texture::bindImage(image_descriptor const &descriptor) const {
	glBindImageTexture(
		descriptor.unit,
		texture_object_,
		descriptor.level,
		descriptor.layered,
		descriptor.layer,
		static_cast<GLenum>(descriptor.access),
		static_cast<GLenum>(descriptor.format)
	); gpu_check;
}

void Texture::bindTextureUnit(u32 const unit) const {
	glBindTextureUnit(unit, texture_object_); gpu_check;
}

void Texture::allocate(glm::ivec2 const &size, i32 const levels, gl::InternalFormat format) {
	internal_format_=format;
	resolution_ = size;
	glTextureStorage2D(texture_object_, levels, static_cast<GLenum>(format), size.x, size.y);
	gpu_check;
}

void Texture::allocate3D(ivec3 const &size, i32 levels, gl::InternalFormat format) {
	internal_format_ = format;
	glTextureStorage3D(texture_object_, levels, static_cast<GLenum>(format), size.x, size.y, size.z);
	gpu_check;
}

void Texture::uploadImage2D(void const *data, i32 const level, glm::ivec2 const &offset, glm::ivec2 const &size, gl::PixelFormat format, gl::PixelType type) {
	glTextureSubImage2D(
		texture_object_, level,
		offset.x, offset.y,
		size.x, size.y,
		static_cast<GLenum>(format),
		static_cast<GLenum>(type),
		data
	);
	this->pixel_format_ = format;
	this->pixel_type_ = type;
	gpu_check;
}

void Texture::setCompressedImage2D(void const *data, i32 const level, glm::ivec2 const &offset, glm::ivec2 const &size, gl::PixelFormat format, gl::sizei_t const pixel_size) {
	assert(data != nullptr);
	glCompressedTextureSubImage2D(
		texture_object_,
		level,
		offset.x, offset.y,
		size.x, size.y,
		static_cast<GLenum>(format),
		pixel_size,
		data
	);
	pixel_format_ = format;
	gpu_check;
}

glm::ivec2 Texture::levelSize2D(i32 const level, glm::ivec2 const &size) const {
	i32 w, h;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_WIDTH, &w); gpu_check;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_HEIGHT, &h);
	return glm::ivec2(w, h);
}

gl::int32_t Texture::compressedImageSize(i32 const level) const {
	i32 size;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size); gpu_check;
	return size;
}

std::vector<u8> Texture::compressedImageData(i32 const level) const {
	i32 size = compressedImageSize(level);
	std::vector<u8> pixels(size);
	glGetCompressedTextureImage(texture_object_, level, size, pixels.data()); gpu_check;
	return pixels;
}

void Texture::compressedImageData(std::vector<u8> &pixels, i32 const level) const {
	assert(compressed(level));
	i32 const size = compressedImageSize(level);
	pixels.resize(size);
	glGetCompressedTextureImage(texture_object_, level, size, pixels.data()); gpu_check;
}

gl::int32_t Texture::imageDataSize(i32 const level) const {
	i32 width, height;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_WIDTH, &width); gpu_check;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_HEIGHT, &height);
	switch (this->pixel_format_) {
		case gl::PixelFormat::Rgb:
			return width * height * 3;
		case gl::PixelFormat::Rgba:
			return width * height * 4;
		default:
			return width * height * 2;
	}
	return 0;
}

void Texture::imageData(std::vector<u8> &pixels, i32 const level) const {
	i32 const size = imageDataSize(level);
	pixels.resize(size);
	glGetTextureImage(
		texture_object_,
		level,
		static_cast<GLenum>(pixel_format_),
		static_cast<GLenum>(pixel_type_),
		size,
		pixels.data()
	);
	gpu_check;
}

bool Texture::compressed(i32 const level) const {
	i32 is_compressed;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_COMPRESSED, &is_compressed);
	return is_compressed == GL_TRUE;
}
gl::InternalFormat Texture::internalFormat() const {
	return static_cast<gl::InternalFormat>(paramInt(gl::GetTextureParameter::TextureInternalFormat));
}
gl::PixelFormat Texture::pixelFormat() const {
	return pixel_format_;
}
gl::PixelType Texture::pixelType() const {
	return pixel_type_;
}
gl::TextureMagFilter Texture::magFilter() const {return static_cast<gl::TextureMagFilter>(paramIntLevel(gl::GetTextureParameter::TextureMagFilter,0)); }
gl::TextureMinFilter Texture::minFilter() const {return static_cast<gl::TextureMinFilter>(paramIntLevel(gl::GetTextureParameter::TextureMinFilter,0));}
gl::TextureWrapMode Texture::wrapMode() const {return static_cast<gl::TextureWrapMode>(paramIntLevel(gl::GetTextureParameter::TextureWrapS,0));}
gl::TextureCompareMode Texture::compareMode() const {return (gl::TextureCompareMode) GL_COMPARE_REF_TO_TEXTURE;}

bool Texture::isValid() const {
	return glIsTexture(texture_object_) == GL_TRUE;
}

void Texture::inspector() {
	using namespace ImGui;
	using namespace gl;

	GLint width, height;
	glGetTextureLevelParameteriv(texture_object_, 0, GL_TEXTURE_WIDTH, &width);
	glGetTextureLevelParameteriv(texture_object_, 0, GL_TEXTURE_HEIGHT, &height);
	GLint intfmt{};
	glGetTextureLevelParameteriv(texture_object_, 0, GL_TEXTURE_INTERNAL_FORMAT, &intfmt);

	auto internal_format{
		static_cast<InternalFormat>(intfmt)
	};
	SeparatorText("Texture Info");
	Text("Resolution: %d x %d", width, height);
	Text("Format: %s", ToString(internal_format));
	
	
	Image(texture_object_, ImVec2(256, 256));
}

void Texture::dispose() {
	glDeleteTextures(1, &texture_object_);
}
bool Texture::disposed() const {
	return glIsTexture(texture_object_) == GL_FALSE;
}