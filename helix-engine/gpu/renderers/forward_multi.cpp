#include "forward_multi.hpp"

#include "ecs/3d/editor/editor_camera.hpp"
#include "engine/main-loop.hpp"
#include "gpu/model_manager.hpp"

#include "glad/glad.h"

ForwardMultiDrawRenderer::ForwardMultiDrawRenderer(SharedPtr<Window> const &window) :
	IRenderer(window), mesh_cull_("shaders\\mesh_cull.comp"), forward_draw_("shaders\\forward_simple.vert", "shaders\\forward_simple.frag"), compositor_(window->getSize()), window_(window) {
	using enum gl::BufferStorageMask;

	constexpr FrameData dummy{};
	frame_data_buffer_.allocateElements(1, &dummy, MapPersistentBit | MapCoherentBit | MapWriteBit);
	frame_data_buffer_.bindToBackedBufferBlock(gl::BufferTargetARB::ShaderStorageBuffer, 16);
	frame_data_ = frame_data_buffer_.mapElementsRange(0, 1, gl::MapBufferAccessMask::MapPersistentBit | gl::MapBufferAccessMask::MapCoherentBit | gl::MapBufferAccessMask::MapFlushExplicitBit | gl::MapBufferAccessMask::MapWriteBit);
}

ForwardMultiDrawRenderer::~ForwardMultiDrawRenderer() {
}

void ForwardMultiDrawRenderer::dispose() {
	compositor_.dispose();
}

bool ForwardMultiDrawRenderer::disposed() const {
	return compositor_.disposed();
}

Result<> ForwardMultiDrawRenderer::resize(ivec2 const desired_resolution) {
	compositor_.resize(desired_resolution);
	return OK;
}

Result<> ForwardMultiDrawRenderer::render() {
	using enum gl::BufferTargetARB;
	
	ModelManager *model_manager = ModelManager::singleton();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//using enum gl::MapBufferAccessMask;
	//assert(glIsBuffer(frame_data_buffer_.buffer_object_));
	//frame_data_ = frame_data_buffer_.mapElementsRange(0, 1, MapPersistentBit | MapCoherentBit | MapWriteBit);

	ivec4 const viewport_size = window_->viewport();
	EditorCamera3D *editor_camera = dynamic_cast<DefMainLoop &>(Main::mainLoop().value()).editor_camera_;
	editor_camera->setFarPlane(4096.0f);
	editor_camera->setNearPlane(0.05f);
	editor_camera->setAspectRatio(static_cast<f32>(viewport_size.z) / static_cast<f32>(viewport_size.w));
	editor_camera->setFov(90.0f);
	editor_camera->refreshMatrices();
	editor_camera->makeCurrent();

	mat4 const projection			= 	editor_camera->projectionMatrix();
	mat4 const view					= 	editor_camera->viewMatrix();
	mat4 const proj_view			= 	editor_camera->projectionViewMatrix();
	mat4 const inverse_projection	= 	editor_camera->inverseProjectionMatrix();
	mat4 const inverse_view			= 	editor_camera->inverseViewMatrix();
	mat4 const inverse_proj_view	= 	editor_camera->inverseProjectionViewMatrix();

	*frame_data_ = FrameData{
		.View = view,
		.Projection = projection,
		.ProjView = proj_view,
		.InvView = inverse_view,
		.InvProjection = inverse_projection,
		.InvProjView = inverse_proj_view,
		.Near = editor_camera->nearPlane(),
		.Far = editor_camera->farPlane(),
		.Time = 0.0f
	};
	//assert(frame_data_buffer_.unmap());
	frame_data_buffer_.flushMappedElementsRange(0, 1);
	model_manager->prerender();

	{
		memset(model_manager->draw_command_count_buffer_.mapped_address_, 0, sizeof(u32) * 8192);
		//model_manager->draw_command_count_buffer_.updateElements(8192, 0, nullptr); //< Clear
	}
	//  u32 zero[1] = {0};
	//  model_manager->draw_command_count_buffer_.updateElements(zero, 0);
	
	mesh_cull_.use();
	mesh_cull_.dispatchCompute(
		static_cast<u32>(std::ceil(static_cast<f32>(model_manager->meshInstanceCount()) / 64.0f)),
		1,
		1
	);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	model_manager->draw_command_buffer_.bind(DrawIndirectBuffer);
	model_manager->draw_command_count_buffer_.bind(ParameterBuffer);

	//static RenderPassInfo compute_pass{
	//	.pass = RenderPassType::CullCompute,
	//	.bind_model_matrix = true,
	//	.shader_program = &mesh_cull_
	//};
	//
	//sceneTree()->initiateRenderSetup(compute_pass);
	//sceneTree()->initiateDraw(compute_pass);

	forward_draw_.use();

	static RenderPassInfo forward_pass{
		.pass = RenderPassType::Normal,
		.bind_model_matrix = true,
		.shader_program = &forward_draw_
	};
	sceneTree()->initiateRenderSetup(forward_pass);
	sceneTree()->initiateDraw(forward_pass);
	
	//ModelManager::singleton()->vao_.bind();
	//glMultiDrawArraysIndirectCount(
	//	GL_TRIANGLES,
	//	nullptr,
	//	0,
	//	8192,
	//	0
	//);0

	window_->swapBuffers();
	
	return OK;
}

SharedPtr<SceneTree> ForwardMultiDrawRenderer::sceneTree() const {
	return window_->sceneTree();
}

RendererType ForwardMultiDrawRenderer::rendererType() const {
	return RendererType::FORWARD_MULTI;
}
