// ReSharper disable CppPassValueParameterByConstReference
#pragma once
#include "renderer.hpp"
#include "gpu/compositor.h"
#include "gpu/geometry_buffer.hpp"

class GBuffer;
class Window;

class DeferredRenderer : public IRenderer {
public:
	DeferredRenderer(SharedPtr<Window> const &window);
	~DeferredRenderer() override;

protected:

	Program write_g_buffer_;
	Program write_depth_;
	Program deferred_lighting_;
	Program screen_space_reflections_;
	Program texture_to_screen_;
	SharedPtr<Mesh> full_screen_quad_;

	RenderPassInfo &renderPass(Program &program) const;
	void swapBuffers() const;

	void writeToGBuffer();
	void deferredLighting();
	void postProcess();

public:
	
	Result<> resize(ivec2) override;
	Result<> render() override;
	
	[[nodiscard]] SharedPtr<SceneTree> sceneTree() const override;

	void dispose() override;
	[[nodiscard]] bool disposed() const override;

private:
	GBuffer g_buffer_;
	Compositor compositor_;
	SharedPtr<Window> window_;
};