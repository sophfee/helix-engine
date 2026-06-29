#include "deferred.hpp"

#include "ecs/3d/editor/editor_camera.hpp"
#include "ecs/core/scene_tree.hpp"
#include "gpu/graphics.hpp"
#include "gpu/lighting.hpp"
#include "gpu/mesh.hpp"
#include "gpu/primitive.hpp"

namespace {
	
}

DeferredRenderer::DeferredRenderer(SharedPtr<Window> const &window)
	: IRenderer(window),
	write_g_buffer_("shaders\\g_buffer_write.vert", "shaders\\g_buffer_write.frag"),
	deferred_lighting_("shaders\\deferred_shading.vert", "shaders\\deferred_shading.frag"),
	texture_to_screen_("shaders\\deferred_shading.vert", "shaders\\texture_to_screen.frag"),
	full_screen_quad_(Primitive::makeQuad(vec2(2.0f), false)),
	g_buffer_(ivec2(window->viewport().z, window->viewport().w)),
	window_(window){
}

DeferredRenderer::~DeferredRenderer() {
	compositor_.dispose();
}

void DeferredRenderer::swapBuffers() const {
	glFlush();
	glfwSwapBuffers(window_->window);
}

void DeferredRenderer::writeToGBuffer() {
	ivec4 const viewport_size = window_->viewport();

	static RenderPassInfo G_BUFFER_PASS{
		.pass = RenderPassType::Normal,
		.model_matrix_location = write_g_buffer_.uniformLocation("model"),
		.view_matrix_location = write_g_buffer_.uniformLocation("view"),
		.projection_matrix_location = write_g_buffer_.uniformLocation("projection"),
		.inverse_view_matrix_location = write_g_buffer_.uniformLocation("inverse_view"),
		.inverse_projection_matrix_location = write_g_buffer_.uniformLocation("inverse_projection"),
		.bind_albedo_texture = true,
		.bind_normal_texture = true,
		.bind_orm_texture = true,
		.bind_object_id = true,
		.frustum_culling = false,
		.render_sky = true,
		.cull = true,
		.bind_time = std::nullopt,
		.viewport = viewport_size,
		.shader_program = &write_g_buffer_,
		.depth = {
			.depth_test = true
		},
		.csm = {
			.bind_buffer = true,
			.buffer_binding = 2,
			.bind_texture_unit = 7,
			.light_direction_location = 22,
			.far_plane_location = 23,
			.world_position_location = 24
		},
		.material_bridge = {
			.diffuse_texture_unit = 0,
			.diffuse_texture_is_used = write_g_buffer_.uniformLocation("u_hasBaseColorTexture"),
			.diffuse_color_modulation = write_g_buffer_.uniformLocation("u_baseColorFactor"),
			.orm_texture_unit = 1,
			.orm_texture_is_used = write_g_buffer_.uniformLocation("u_hasMetallicRoughnessTexture"),
			.normal_texture_unit = 2,
			.normal_texture_is_used = write_g_buffer_.uniformLocation("u_hasNormalTexture"),
			.emissive_texture_unit = 3,
			.emissive_texture_is_used = write_g_buffer_.uniformLocation("u_hasEmissiveTexture"),
			.emissive_color_modulation = write_g_buffer_.uniformLocation("u_emissiveBias")
		}
	};

	EditorCamera3D *editor_camera = dynamic_cast<DefMainLoop &>(Main::mainLoop().value()).editor_camera_;
	editor_camera->setFarPlane(4096.0f);
	editor_camera->setNearPlane(0.05f);
	editor_camera->setAspectRatio(static_cast<f32>(viewport_size.z) / static_cast<f32>(viewport_size.w));
	editor_camera->setFov(90.0f);
	editor_camera->refreshMatrices();
	editor_camera->makeCurrent();

	write_g_buffer_.use();
	
	g_buffer_.bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	sceneTree()->initiateRenderSetup(G_BUFFER_PASS);
	sceneTree()->initiateDraw(G_BUFFER_PASS);

	g_buffer_.unbind();
}

void DeferredRenderer::deferredLighting() {
	ivec4 const viewport_size = window_->viewport();
	
	static RenderPassInfo DEFERRED_PASS{
		.pass = RenderPassType::Normal,
		.model_matrix_location = -1,
		.view_matrix_location = deferred_lighting_.uniformLocation("view"),
		.projection_matrix_location = deferred_lighting_.uniformLocation("projection"),
		.inverse_view_matrix_location = deferred_lighting_.uniformLocation("inverse_view"),
		.inverse_projection_matrix_location = deferred_lighting_.uniformLocation("inverse_projection"),
		.bind_albedo_texture = false,
		.bind_normal_texture = false,
		.bind_orm_texture = false,
		.bind_object_id = false,
		.frustum_culling = false,
		.render_sky = true,
		.cull = false,
		.bind_time = std::nullopt,
		.viewport = viewport_size,
		.shader_program = &deferred_lighting_,
		.depth = {
			.depth_test = false
		},
		.csm = {
			.bind_buffer = true,
			.buffer_binding = 2,
			.bind_texture_unit = 7,
			.light_direction_location = 22,
			.far_plane_location = 23,
			.world_position_location = 24
		}
	};

	LightingSystem::singleton()->prerender();

	compositor_.beginDraw();
	
	deferred_lighting_.use();

	auto const &def_main_loop = dynamic_cast<DefMainLoop &>(Main::mainLoop().value());
	
	deferred_lighting_.setUniform(4, def_main_loop.editor_camera_->inverseProjectionMatrix());
	deferred_lighting_.setUniform(5, def_main_loop.editor_camera_->inverseViewMatrix());
	deferred_lighting_.setUniform(6, def_main_loop.editor_camera_->projectionMatrix());
	deferred_lighting_.setUniform(7, def_main_loop.editor_camera_->viewMatrix());

	g_buffer_.color().bindTextureUnit(0);
	g_buffer_.position().bindTextureUnit(1);
	g_buffer_.normal().bindTextureUnit(2);
	g_buffer_.orm().bindTextureUnit(3);
	g_buffer_.id().bindTextureUnit(4);
	g_buffer_.emissive().bindTextureUnit(5);

	SceneTree::setupRenderPass(DEFERRED_PASS);
	full_screen_quad_->drawAllSubMeshes(DEFERRED_PASS);

	compositor_.endDraw();
}

void DeferredRenderer::postProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	texture_to_screen_.use();
	compositor_.bindRenderOutputToUnit(0);

	ivec4 const viewport_size = window_->viewport();
	static RenderPassInfo DEFERRED_PASS{
		.pass = RenderPassType::Normal,
		.model_matrix_location = -1,
		.view_matrix_location = deferred_lighting_.uniformLocation("view"),
		.projection_matrix_location = deferred_lighting_.uniformLocation("projection"),
		.inverse_view_matrix_location = deferred_lighting_.uniformLocation("inverse_view"),
		.inverse_projection_matrix_location = deferred_lighting_.uniformLocation("inverse_projection"),
		.bind_albedo_texture = false,
		.bind_normal_texture = false,
		.bind_orm_texture = false,
		.bind_object_id = false,
		.frustum_culling = false,
		.render_sky = true,
		.cull = false,
		.bind_time = std::nullopt,
		.viewport = viewport_size,
		.shader_program = &texture_to_screen_,
		.depth = {
			.depth_test = false
		},
		.csm = {
			.bind_buffer = true,
			.buffer_binding = 2,
			.bind_texture_unit = 7,
			.light_direction_location = 22,
			.far_plane_location = 23,
			.world_position_location = 24
		}
	};
	SceneTree::setupRenderPass(DEFERRED_PASS);
	full_screen_quad_->drawAllSubMeshes(DEFERRED_PASS);
}

Result<> DeferredRenderer::resize(ivec2 const resolution) {
	compositor_.resize(resolution);
	g_buffer_.changeResolution(resolution);
	return OK;
}

Result<> DeferredRenderer::render() {

	SharedPtr<SceneTree> scene_tree = sceneTree();

	ivec2 const resolution = window_->getSize();

	if (compositor_.resolution() != resolution) {
		resize(resolution);
	}
	
	compositor_.integrityCheck();
	write_g_buffer_.integrityCheck();
	write_depth_.integrityCheck();
	deferred_lighting_.integrityCheck();
	texture_to_screen_.integrityCheck();

	writeToGBuffer();
	deferredLighting();
	postProcess();

	swapBuffers();
	
	return OK;
}

SharedPtr<SceneTree> DeferredRenderer::sceneTree() const {
	return window_->sceneTree();
}

void DeferredRenderer::dispose() {
	compositor_.dispose();
	write_g_buffer_.dispose();
	write_depth_.dispose();
	deferred_lighting_.dispose();
	if (!window_->disposed())
		window_->dispose();
}

bool DeferredRenderer::disposed() const {
	return compositor_.disposed()
		&& write_g_buffer_.disposed() 
		&& write_depth_.disposed() 
		&& deferred_lighting_.disposed();
}
