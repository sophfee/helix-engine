#pragma once
#include "math.hpp"
#include "types.hpp"
#include "engine/disposable.hpp"
#include "texture.h"

class Framebuffer;

class Renderbuffer : public IDisposable {
	u32 renderbuffer_object_;
public:
	Renderbuffer();
	~Renderbuffer() override;
	Renderbuffer(Renderbuffer const &) = delete;
	Renderbuffer &operator=(Renderbuffer const &p) = delete;

	void allocateStorage(::ivec2 const &size, gl::InternalFormat internalFormat) const;
	void allocateStorageMultisample(ivec2 const &size, i32 samples, gl::InternalFormat internalFormat) const;
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

	friend class Framebuffer;
};

class FramebufferBuilder {
	struct Attachment {
		std::reference_wrapper<Texture> texture;
		gl::FramebufferAttachment attachment;
		int level = 0;
		int layer = 0;
	};
	Vec<std::reference_wrapper<Texture>> textures_{};
public:
	FramebufferBuilder() = default;

	FramebufferBuilder &attach(gl::FramebufferAttachment attachment, Texture const &texture, int level = 0, int layer = 0);
	FramebufferBuilder &attach(gl::FramebufferAttachment attachment, Texture const &texture);
	FramebufferBuilder &attach(Texture const &texture);
	//FramebufferBuilder &attach(Texture const &texture, Texture const &...other);
	
	Box<Framebuffer> instantiateAsUnique();
};

class Framebuffer : public IDisposable {
	Box<Renderbuffer> renderbuffer_; // allows the framebuffer to own one :)
	static u32 bound_framebuffer_;
	static u32 bound_draw_framebuffer_;
	static u32 bound_read_framebuffer_;
public:
	u32 framebuffer_object_;
	
	Framebuffer();
	Framebuffer(u32 index);
	~Framebuffer() override;
	Framebuffer(Framebuffer const &) = delete;
	Framebuffer &operator=(Framebuffer const &p) = delete;
	void bind(gl::FramebufferTarget target = gl::FramebufferTarget::Framebuffer) const;
	void unbind(gl::FramebufferTarget target = gl::FramebufferTarget::Framebuffer) const;
	void setLabel(_STD string_view const p_label) const;

	struct AttachmentTextureLevel {
		gl::FramebufferAttachment attachment;
		std::reference_wrapper<Texture> texture;
		i32 level = 0;
	};

	struct AttachmentRenderBuffer {
		gl::FramebufferAttachment attachment;
		std::reference_wrapper<Renderbuffer> renderbuffer;
	};

	using GenericAttachment = Variant<AttachmentTextureLevel, AttachmentRenderBuffer>;
	
	void attachTexture(gl::FramebufferAttachment attachment, Texture const &texture, i32 level = 0) const;
	void attachTextureLayer(gl::FramebufferAttachment attachment, Texture const &texture, i32 level, i32 layer) const;
	void attachTextures(Vec<gl::FramebufferAttachment> const &attachments, Vec<std::reference_wrapper<Texture>> const &textures, i32 level = 0) const;
	void attachTextures(Vec<AttachmentTextureLevel> const &attachments) const;
	void attachRenderbuffer(Renderbuffer const &renderbuffer, gl::FramebufferAttachment attachment = gl::FramebufferAttachment::DepthStencilAttachment) const;

	void attach(Vec<GenericAttachment> const &attachments) const;

	void clear(gl::Buffer buffer, int draw_buffer_index = 0) const;
	
	void setDrawBuffers(_STD vector<gl::ColorBuffer> const &buffers) const;
	void setReadBuffer(Optional<gl::ColorBuffer> buffer) const;
	_NODISCARD gl::FramebufferStatus status() const;

	void blit(Framebuffer const &dest, ivec4 const &src, ivec4 const &dst, gl::bitfield_t mask, gl::BlitFramebufferFilter filter) const;
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
};

extern Framebuffer default_framebuffer;
constexpr gl::int32_t model_matrix_location = 0;