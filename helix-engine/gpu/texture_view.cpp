#include "texture_view.h"

#include "texture.h"
#include "glad/glad.h"

using namespace helix;
using namespace helix::render;

TextureView::TextureView(u32 texture_view_object): texture_view_object_(texture_view_object) {
}

TextureView::TextureView(TextureView const &rhs) :
	original_texture_object_(rhs.original_texture_object_),
	target_(rhs.target_),
	internal_format_(rhs.internal_format_),
	base_level_(rhs.base_level_),
	num_levels_(rhs.num_levels_),
	base_layer_(rhs.base_layer_),
	num_layers_(rhs.num_layers_),
	exists_(rhs.exists_) {
	glGenTextures(1, &texture_view_object_);
	glTextureView(texture_view_object_, target_,
		original_texture_object_,
		internal_format_,
		base_level_,
		num_levels_,
		base_layer_,
		num_layers_
	);
}

TextureView::TextureView(TextureView &&rhs) noexcept :
	original_texture_object_(rhs.original_texture_object_),
	texture_view_object_(rhs.texture_view_object_),
	target_(rhs.target_),
	internal_format_(rhs.internal_format_),
	base_level_(rhs.base_level_),
	num_levels_(rhs.num_levels_),
	base_layer_(rhs.base_layer_),
	num_layers_(rhs.num_layers_),
	exists_(rhs.exists_) {
	rhs.original_texture_object_ = 0;
	rhs.texture_view_object_ = 0;
	rhs.target_ = 0;
	rhs.internal_format_ = 0;
	rhs.base_level_ = 0;
	rhs.num_levels_ = 0;
	rhs.base_layer_ = 0;
	rhs.num_layers_ = 0;
}

TextureView & TextureView::operator=(TextureView const &other) {
	if (this == &other)
		return *this;

	if (exists_)
		glDeleteTextures(1, &texture_view_object_);

	if (!other.exists_)
		return *this;

	this->original_texture_object_ = other.original_texture_object_;
	this->target_ = other.target_;
	this->internal_format_ = other.internal_format_;
	this->base_level_ = other.base_level_;
	this->num_levels_ = other.num_layers_;
	this->base_layer_ = other.base_layer_;
	this->num_layers_ = other.num_layers_;

	this->exists_ = true;

	glGenTextures(1, &texture_view_object_);
	glTextureView(texture_view_object_, target_,
		original_texture_object_,
		internal_format_,
		base_level_,
		num_levels_,
		base_layer_,
		num_layers_
	);
	
	return *this;
}

TextureView & TextureView::operator=(TextureView &&rhs) noexcept {
	if (this == &rhs)
		return *this;

	if (exists_)
		glDeleteTextures(1, &texture_view_object_);

	if (!rhs.exists_)
		return *this;

	this->original_texture_object_ = rhs.original_texture_object_;
	this->target_ = rhs.target_;
	this->internal_format_ = rhs.internal_format_;
	this->base_level_ = rhs.base_level_;
	this->num_levels_ = rhs.num_layers_;
	this->base_layer_ = rhs.base_layer_;
	this->num_layers_ = rhs.num_layers_;
	this->exists_ = rhs.exists_;
	rhs.original_texture_object_ = 0;
	rhs.target_ = 0;
	rhs.internal_format_ = 0;
	rhs.base_level_ = 0;
	rhs.num_levels_ = 0;
	rhs.base_layer_ = 0;
	rhs.num_layers_ = 0;
	rhs.exists_ = false;
	return *this;
}
TextureView::~TextureView() {
	if (!TextureView::disposed())
		TextureView::dispose();
}
void TextureView::dispose() {
	glDeleteTextures(1, &texture_view_object_);
}
bool TextureView::disposed() const {
	return glIsTexture(texture_view_object_) == GL_FALSE;
}
