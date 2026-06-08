#pragma once
#include "types.hpp"

class Texture;
class Shader;
class Program;
class Buffer;
class VertexArray;
class Renderbuffer;
class Framebuffer;

class RenderServer {

	Vec<Texture *> textures_;
	UnorderedMap<String, Shader *> shaders_;
	Vec<Program *> programs_;
	Vec<Buffer *> buffers_;
	Vec<VertexArray *> vaos_;
	Vec<Framebuffer *> framebuffers_;
	Vec<Renderbuffer *> renderbuffers_;
	
	RenderServer();
public:
	static RenderServer &singleton();

	~RenderServer();

	RenderServer(RenderServer const &) = delete;
	RenderServer &operator=(RenderServer const &) = delete;

	RenderServer(RenderServer &&) noexcept = delete;
	RenderServer &operator=(RenderServer &&) noexcept = delete;

	void track(Texture *texture);
	void track(String const &path, Shader *shader);
	void track(Program *program);
	void track(Buffer *buffer);
	void track(VertexArray *vertexArray);
	void track(Renderbuffer *renderbuffer);
	void track(Framebuffer *framebuffer);

	void prune();

	void shutdown();
};