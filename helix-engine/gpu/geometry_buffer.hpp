#pragma once

#include "texture.h"
#include "framebuffer.h"

namespace helix::render {
	class GBuffer {
		struct Storage {
			Framebuffer fb;
			Renderbuffer rb;
			Texture color;
			Texture normal;
			Texture position;
			Texture orm;
			Texture id;
			Texture emissive;
		};
		Box<Storage> storage;
	
	public:
		GBuffer();
		GBuffer(ivec2 const &resolution);
		GBuffer(GBuffer const &) = delete;
		GBuffer(GBuffer &&) = delete;
		GBuffer& operator=(GBuffer const &) = delete;
		GBuffer& operator=(GBuffer &&) = delete;
		~GBuffer();

		void changeResolution(ivec2 resolution);

		void bind() const;
		void unbind() const;
		void bindForDrawing() const;
		void unbindForDrawing() const;
		void bindForReading() const;
		void unbindForReading() const;

		[[nodiscard]] Texture const &color() const;
		[[nodiscard]] Texture const &normal() const;
		[[nodiscard]] Texture const &position() const;
		[[nodiscard]] Texture const &orm() const;
		[[nodiscard]] Texture const &id() const;
		[[nodiscard]] Texture const &emissive() const;
	};
}