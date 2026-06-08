#include "framebuffer.h"

#include "graphics.hpp"
#include "render_server.h"
#include "texture.h"
#include "glad/glad.h"

Renderbuffer::Renderbuffer() : renderbuffer_object_(0u) {
	glCreateRenderbuffers(1, &renderbuffer_object_);
	RenderServer::singleton().track(this);
}

Renderbuffer::~Renderbuffer() {
	glDeleteRenderbuffers(1, &renderbuffer_object_);
}

void Renderbuffer::allocateStorage(glm::ivec2 const &size, gl::InternalFormat internalFormat) const {
	glNamedRenderbufferStorage(
		renderbuffer_object_,
		static_cast<GLenum>(internalFormat),
		size.x, size.y
	); gpu_check;
}

void Renderbuffer::allocateStorageMultisample(glm::ivec2 const &size, i32 const samples, gl::InternalFormat internalFormat) const {
	glNamedRenderbufferStorageMultisample(
		renderbuffer_object_,
		samples,
		static_cast<GLenum>(internalFormat),
		size.x, size.y
	); gpu_check;
}
void Renderbuffer::dispose() {
	glDeleteRenderbuffers(1, &renderbuffer_object_);
}
bool Renderbuffer::disposed() const {
	return glIsRenderbuffer(renderbuffer_object_) == GL_FALSE;
}

u32 Framebuffer::bound_framebuffer_ = 0xFFFFFFFFu;
u32 Framebuffer::bound_draw_framebuffer_ = 0xFFFFFFFFu;
u32 Framebuffer::bound_read_framebuffer_ = 0xFFFFFFFFu;

Framebuffer::Framebuffer(u32 const index) : framebuffer_object_(index) {
	RenderServer::singleton().track(this);
}

Framebuffer::Framebuffer() {
	glCreateFramebuffers(1, &framebuffer_object_);
	RenderServer::singleton().track(this);
}

Framebuffer::~Framebuffer() {
	if (framebuffer_object_ != 0 && framebuffer_object_ != 0xFFFFFFFFu) { //< Don't delete the default framebuffer
		glDeleteFramebuffers(1, &framebuffer_object_);
	}
}

void Framebuffer::bind(gl::FramebufferTarget target) const {
	/*
	switch (target) {
		case gl::FramebufferTarget::DrawFramebuffer:
			if (bound_draw_framebuffer_ == framebuffer_object_)
				return;
			bound_draw_framebuffer_ = framebuffer_object_;
			break;
		case gl::FramebufferTarget::ReadFramebuffer:
			if (bound_read_framebuffer_ == framebuffer_object_)
				return;
			bound_read_framebuffer_ = framebuffer_object_;
			break;
		case gl::FramebufferTarget::Framebuffer:
			if (bound_framebuffer_ == framebuffer_object_)
				return;
			bound_framebuffer_ = framebuffer_object_;
			break;
	}
	*/
	glBindFramebuffer(static_cast<gl::enum_t>(target), framebuffer_object_);
}

void Framebuffer::unbind(gl::FramebufferTarget target) const {
	switch (target) {
		case gl::FramebufferTarget::DrawFramebuffer:
			if (bound_draw_framebuffer_ == 0 || bound_draw_framebuffer_ != framebuffer_object_)
				return;
			bound_draw_framebuffer_ = 0;
			break;
		case gl::FramebufferTarget::ReadFramebuffer:
			if (bound_read_framebuffer_ == 0 || bound_read_framebuffer_ != framebuffer_object_)
				return;
			bound_read_framebuffer_ = 0;
			break;
		case gl::FramebufferTarget::Framebuffer:
			if (bound_framebuffer_ == 0 || bound_framebuffer_ != framebuffer_object_)
				return;
			bound_framebuffer_ = 0;
			break;
	}
	glBindFramebuffer(static_cast<gl::enum_t>(target), 0);
}
void Framebuffer::setLabel(_STD string_view const p_label) const {
	glObjectLabel(GL_FRAMEBUFFER, framebuffer_object_, static_cast<GLsizei>(p_label.size()), p_label.data());
}

void Framebuffer::attachTexture(gl::FramebufferAttachment attachment, Texture const &texture, i32 const level) const {
	glNamedFramebufferTexture(
		framebuffer_object_,
		static_cast<GLenum>(attachment),
		texture.texture_object_,
		level
	);
}

void Framebuffer::attachTextures(Vec<gl::FramebufferAttachment> const &attachments, Vec<std::reference_wrapper<Texture>> const &textures, i32 level) const {
	assert(attachments.size() == textures.size());
	for (size_t i = 0; i < attachments.size(); ++i)
		attachTexture(attachments[i], textures[i].get(), level);
}

void Framebuffer::attachTextures(Vec<AttachmentTextureLevel> const &attachments) const {
	for (size_t i = 0; i < attachments.size(); ++i)
		attachTexture(attachments[i].attachment,
		              attachments[i].texture.get(),
		              attachments[i].level);
}

void Framebuffer::attachRenderbuffer(Renderbuffer const &renderbuffer, gl::FramebufferAttachment attachment) const {
	glNamedFramebufferRenderbuffer(
		framebuffer_object_,
		static_cast<GLenum>(attachment),
		GL_RENDERBUFFER,
		renderbuffer.renderbuffer_object_
	);
}

void Framebuffer::attach(Vec<GenericAttachment> const &attachments) const {
	for (GenericAttachment const &attachment : attachments) {
		if (std::holds_alternative<AttachmentRenderBuffer>(attachment)) {
			AttachmentRenderBuffer const &renderbuffer = std::get<AttachmentRenderBuffer>(attachment);
			attachRenderbuffer(renderbuffer.renderbuffer.get(), renderbuffer.attachment);
		}
		else {
			AttachmentTextureLevel const &texture = std::get<AttachmentTextureLevel>(attachment);
			attachTexture(texture.attachment, texture.texture.get(), texture.level);
		}
	}
}

void Framebuffer::setDrawBuffers(_STD vector<gl::ColorBuffer> const &buffers) const {
	if (buffers.empty()) {
		glNamedFramebufferDrawBuffer(framebuffer_object_, GL_NONE);
		return;
	}

	if (buffers.size() == 1) {
		glNamedFramebufferDrawBuffer(framebuffer_object_, static_cast<GLenum>(buffers[0]));
		return;
	}
	
	glNamedFramebufferDrawBuffers(
		framebuffer_object_,
		static_cast<GLsizei>(buffers.size()),
		reinterpret_cast<GLenum const *>(buffers.data())
	);
}

void Framebuffer::setReadBuffer(Optional<gl::ColorBuffer> buffer) const {
	glNamedFramebufferReadBuffer(
		framebuffer_object_,
		buffer.has_value() ? static_cast<GLenum>(buffer.value()) : GL_NONE
	);
}

gl::FramebufferStatus Framebuffer::status() const {
	gl::enum_t status = glCheckNamedFramebufferStatus(framebuffer_object_, GL_FRAMEBUFFER);
	return static_cast<gl::FramebufferStatus>(status);
}

void Framebuffer::blit(Framebuffer const &dest, glm::ivec4 const &src, glm::ivec4 const &dst, gl::bitfield_t const mask, gl::BlitFramebufferFilter filter) const {
	glBlitNamedFramebuffer(
		framebuffer_object_,
		dest.framebuffer_object_,
		src.x, src.y, src.z, src.w,
		dst.x, dst.y, dst.z, dst.w,
		mask,
		static_cast<GLenum>(filter)
	);
}

void Framebuffer::dispose() {
	glDeleteFramebuffers(1, &framebuffer_object_);
}
bool Framebuffer::disposed() const {
	return glIsFramebuffer(framebuffer_object_) == GL_FALSE;
}

Framebuffer default_framebuffer(0);