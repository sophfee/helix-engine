#include "render_server.h"

#include "buffer.h"
#include "framebuffer.h"
#include "graphics.hpp"
#include "texture.h"

using namespace helix;
using namespace helix::render;

RenderServer::RenderServer() {
}
RenderServer & RenderServer::singleton() {
	static RenderServer single;
	return single;
}

RenderServer::~RenderServer() {
	
}

void RenderServer::track(Texture *texture) {
}

void RenderServer::track(String const &path, Shader *shader) {
	shaders_[path] = shader;
}

void RenderServer::track(Program *program) {
}

void RenderServer::track(Buffer *buffer) {
}

void RenderServer::track(VertexArray *vertexArray) {
}
void RenderServer::track(Renderbuffer *renderbuffer) {
}
void RenderServer::track(Framebuffer *framebuffer) {
}

void RenderServer::prune() {
	// std::erase_if(shaders_, [](auto const &p) { return p.second == nullptr || p.second->disposed(); });
	// 
	// std::erase_if(textures_, [](Texture const *t) { return t == nullptr || t->disposed(); });
	// textures_.shrink_to_fit();
	// 
	// std::erase_if(programs_, [](Program const *p) { return p == nullptr || p->disposed(); });
	// programs_.shrink_to_fit();
	// 
	// std::erase_if(buffers_, [](Buffer const *b) { return b == nullptr; });
	// buffers_.shrink_to_fit();
	// 
	// std::erase_if(vaos_, [](VertexArray const *v) { return v == nullptr || v->disposed(); });
	// vaos_.shrink_to_fit();
	// 
	// std::erase_if(renderbuffers_, [](Renderbuffer const *r) { return r == nullptr; });
	// renderbuffers_.shrink_to_fit();
	// 
	// std::erase_if(framebuffers_, [](Framebuffer const *f) { return f == nullptr; });
	// framebuffers_.shrink_to_fit();
}
void RenderServer::shutdown() {
	using namespace std::ranges;
	
	for_each(textures_, [](Texture *t) { if (_CrtIsValidHeapPointer(t) && t) { t->dispose(); delete t; } });
	//textures_.clear();

	for_each(shaders_, [](auto const &p) { if (_CrtIsValidHeapPointer(p.second) && p.second) { p.second->dispose(); delete p.second; } });
	//shaders_.clear();

	for_each(programs_, [](Program *p) { if (_CrtIsValidHeapPointer(p) && p) { p->dispose(); delete p; } });
	//programs_.clear();

	for_each(buffers_, [](Buffer *b) { if (_CrtIsValidHeapPointer(b) && b) { delete b; } });
	//buffers_.clear();

	for_each(vaos_, [](VertexArray *v) { if (_CrtIsValidHeapPointer(v) && v) { delete v; } });
	//vaos_.clear();

	for_each(renderbuffers_, [](Renderbuffer *r) { if (_CrtIsValidHeapPointer(r) && r) { delete r; } });
	//renderbuffers_.clear();

	for_each(framebuffers_, [](Framebuffer *f) { if (_CrtIsValidHeapPointer(f) && f) { delete f; } });
	//framebuffers_.clear();
}
