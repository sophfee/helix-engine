#pragma once
#include "math.hpp"
#include "opengl_enums.hpp"
#include "types.hpp"


struct image_descriptor {
	gl::InternalFormat  format;
	gl::BufferAccessARB access;
	gl::uint32_t unit = 0;
	gl::int32_t level = 0;
	gl::int32_t layer = 0;
	bool layered = false;
};

template <gl::TextureTarget T>
struct TextureBuilder {
	ivec2 image_resolution = ivec2(1, 1);
	i32 layer_count = 1;
	i32 levels = 1;
	gl::PixelFormat pixel_format = gl::PixelFormat::Rgb;
	gl::PixelType pixel_type = gl::PixelType::UnsignedByte;
	gl::InternalFormat internal_format = gl::InternalFormat::Rgba8;
	gl::TextureWrapMode wrap_mode = gl::TextureWrapMode::ClampToEdge;
	gl::TextureMagFilter mag_filter = gl::TextureMagFilter::Linear;
	gl::TextureMinFilter min_filter = gl::TextureMinFilter::Linear;
	gl::TextureCompareMode compare_mode = gl::TextureCompareMode::None;
	gl::CompareFunction compare_function = gl::CompareFunction::Always;
	vec4 border_color = vec4(0, 0, 0, 0);
	bool generate_mipmaps = false;
	bool anisotropic_filtering = false;
	Optional<void *> data;

	TextureBuilder &resolution(ivec2 const &p_resolution) {
		this->image_resolution = p_resolution;
		return *this;
	}

	TextureBuilder &layers(i32 const p_layers)  {
		this->layer_count = p_layers;
		return *this;
	}

	TextureBuilder &withLevels(i32 p_levels) {
		this->levels = p_levels;
		return *this;
	}

	TextureBuilder &pixelFormat(gl::PixelFormat const format) {
		this->pixel_format = format;
		return *this;
	}

	TextureBuilder &pixelType(gl::PixelType const type) {
		this->pixel_type = type;
		return *this;
	}
	
	TextureBuilder &internalFormat(gl::InternalFormat const format) {
		this->internal_format = format;
		return *this;
	}

	TextureBuilder &wrapMode(gl::TextureWrapMode const wrap) {
		this->wrap_mode = wrap;
		return *this;
	}

	TextureBuilder &magFilter(gl::TextureMagFilter const filter) {
		this->mag_filter = filter;
		return *this;
	}

	TextureBuilder &minFilter(gl::TextureMinFilter const filter) {
		this->min_filter = filter;
		return *this;
	}

	TextureBuilder &filter(gl::TextureMinFilter const p_min_filter, gl::TextureMagFilter const p_mag_filter) {
		this->min_filter = p_min_filter;
		this->mag_filter = p_mag_filter;
		return *this;
	}

	TextureBuilder &compareMode(gl::TextureCompareMode const p_compare_mode) {
		this->compare_mode = p_compare_mode;
		return *this;
	}

	TextureBuilder &compareFunc(gl::CompareFunction const p_compare_function) {
		this->compare_function = p_compare_function;
		return *this;
	}

	TextureBuilder &borderColor(vec4 p_border_color) {
		this->border_color = p_border_color;
		return *this;
	}

	TextureBuilder &shouldGenerateMipmaps(bool yesno) {
		this->generate_mipmaps = yesno;
		return *this;
	}

	TextureBuilder &anisotropic(bool yesno) {
		this->anisotropic_filtering = yesno;
		return *this;
	}

	TextureBuilder &imageData(void *p) {
		this->data = p;
		return *this;
	}
};

using TextureBufferBuilder = TextureBuilder<gl::TextureTarget::TextureBuffer>;
using TextureRectangleBuilder = TextureBuilder<gl::TextureTarget::TextureRectangle>;

using Texture1DBuilder = TextureBuilder<gl::TextureTarget::Texture1D>;
using Texture1DArrayBuilder = TextureBuilder<gl::TextureTarget::Texture1DArray>;

using Texture2DBuilder = TextureBuilder<gl::TextureTarget::Texture2D>;
using Texture2DArrayBuilder = TextureBuilder<gl::TextureTarget::Texture2DArray>;

using Texture3DBuilder = TextureBuilder<gl::TextureTarget::Texture3D>;
using TextureCubeMapBuilder = TextureBuilder<gl::TextureTarget::TextureCubeMap>;

namespace detail {
	constexpr bool textureTarget3D(gl::TextureTarget target) {
		return target == gl::TextureTarget::Texture3D || target == gl::TextureTarget::Texture2DArray || target == gl::TextureTarget::TextureCubeMap;
	}

	constexpr bool textureTarget2D(gl::TextureTarget target) {
		return target == gl::TextureTarget::Texture2D || target == gl::TextureTarget::TextureRectangle;
	}
}

class Texture {
	static u32 bound_texture_2d_;
	bool is_dsa_ = true;
	gl::InternalFormat internal_format_;
	gl::PixelFormat pixel_format_;
	gl::PixelType pixel_type_;
	bool anisotropic_filtering_enabled_ = false;

	void createObject(gl::TextureTarget target);

public:
	u32 texture_object_;

	template <gl::TextureTarget T>
	Texture(TextureBuilder<T> const &settings) : internal_format_(settings.internal_format),
		pixel_format_(settings.pixel_format), pixel_type_(settings.pixel_type),
		anisotropic_filtering_enabled_(settings.anisotropic_filtering)
	{
		createObject(T);
		if constexpr (detail::textureTarget2D(T))
			allocate(settings.image_resolution, settings.levels, settings.internal_format);
		else if constexpr (detail::textureTarget3D(T))
			allocate3D(ivec3(settings.image_resolution, settings.layer_count), settings.levels, settings.internal_format);

		if (settings.data.has_value())
			uploadImage2D(settings.data.value(), 0, ivec2(0, 0), settings.image_resolution, settings.pixel_format, settings.pixel_type);

		setWrapMode(settings.wrap_mode);
		setFilter(settings.min_filter, settings.mag_filter);
		setCompareMode(settings.compare_mode);
		setCompareFunction(settings.compare_function);
		setBorderColor(settings.border_color);
		
		if (settings.generate_mipmaps)
			generateMipmap();
		if (settings.anisotropic_filtering)
			enableAnisotropicFiltering();
	}
	
	Texture(gl::TextureTarget p_textureTarget);
	Texture(u32 existing_texture_object_);
	~Texture();

	Texture(Texture const& p_texture) = delete;
	Texture(Texture&& p_texture) = delete;
	Texture& operator=(Texture const& p_texture) = delete;
	Texture& operator=(Texture&& p_texture) = delete;

	void bind(gl::TextureTarget target) const;

	void setLabel(_STD string_view name) const;

	_NODISCARD i32 intParam(gl::GetTextureParameter parameter) const;
	void setIntParam(gl::GetTextureParameter parameter, i32 value) const;

	_NODISCARD u32 uintParam(gl::GetTextureParameter parameter) const;
	void setUIntParam(gl::GetTextureParameter parameter, u32 value) const;
	
	_NODISCARD _STD vector<i32> intVecParam(gl::GetTextureParameter parameter) const;
	void setIntVecParam(gl::GetTextureParameter parameter, _STD vector<i32> const& value) const;

	void setWrapMode(gl::TextureWrapMode wrap_mode, std::optional<gl::TextureWrapMode> wrap_s = std::nullopt, std::optional<gl::TextureWrapMode> wrap_t = std::nullopt) const;
	void setFilter(gl::TextureMinFilter min_filter, gl::TextureMagFilter mag_filter) const;
	void setBorderColor(vec4 const &color) const;
	void setCompareMode(gl::TextureCompareMode compare_mode) const;
	void setCompareFunction(gl::CompareFunction compare_function) const;

	_NODISCARD f32 getFloatParam(gl::GetTextureParameter parameter) const;
	void setFloatParam(gl::GetTextureParameter parameter, f32 value) const;

	_NODISCARD vec4 borderColor() const;
	void generateMipmap() const;
	void setAnisotropicFilteringEnabled(bool enabled);
	void enableAnisotropicFiltering();
	void disableAnisotropicFiltering();
	_NODISCARD bool isAnisotropicFilteringEnabled() const;

	void bindImage(gl::uint32_t unit, gl::InternalFormat format, gl::BufferAccessARB access, gl::int32_t level = 0, bool layered = false, gl::int32_t layer = 0) const;
	void bindImage(image_descriptor const& descriptor) const;
	void bindTextureUnit(u32 unit) const;

	void allocate(::ivec2 const &size, i32 levels, gl::InternalFormat format);
	void allocate3D(::ivec3 const &size, i32 levels, gl::InternalFormat format);
	void uploadImage2D(void const *data, i32 level, ivec2 const &offset, ivec2 const &size, gl::PixelFormat format = gl::PixelFormat::Rgba, gl::PixelType type = gl::PixelType::Byte);
	void setCompressedImage2D(void const *data, i32 level, ivec2 const &offset, ivec2 const &size, gl::PixelFormat format = gl::PixelFormat::Rgba, gl::sizei_t pixel_size = 0);

	_NODISCARD ivec2 levelSize2D(i32 level, ivec2 const &size) const;
	_NODISCARD gl::int32_t compressedImageSize(i32 level = 0) const;
	_NODISCARD _STD vector<u8> compressedImageData(i32 level = 0) const;
	void compressedImageData(_STD vector<u8> &pixels, i32 level = 0) const;

	_NODISCARD gl::int32_t imageDataSize(i32 level = 0) const;
	void imageData(_STD vector<u8> &pixels, i32 level = 0) const;

	_NODISCARD bool compressed(i32 level = 0) const;

	_NODISCARD bool isValid() const;

private:
	template <gl::GetTextureParameter P, typename T>
	void setParamI(T value) const {
		setIntParam(P, static_cast<i32>(value));
	}
public:
	friend class Framebuffer;
};