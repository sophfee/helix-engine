// ReSharper disable CppCStyleCast
#ifdef _DEBUG
#define STBI_FAILURE_USERMSG
#endif
#define STB_IMAGE_IMPLEMENTATION

#include <chrono>
#include <iostream>
#include <thread>
#include <bit>
#include <random>

#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>

#include "os.hpp"
#include "util.hpp"

#include "ecs/ecs.hpp"
#include "ecs/ecs_gltf.hpp"
#include "ecs/light.hpp"
#include "ecs/transform.h"
#include "ecs/3d/camera.hpp"
#include "ecs/3d/editor/editor_camera.hpp"
#include "ecs/3d/effects/environment.hpp"
#include "ecs/3d/lights/directional_light.hpp"
#include "ecs/core/scene_tree.hpp"
#include "engine/filesystem.hpp"
#include "engine/Input.h"

#include "gpu/graphics.hpp"
#include "gpu/mesh.hpp"
#include "gpu/gltf.h"
#include "gpu/png.hpp"
#include "gpu/texture.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "stb/stb_image.h"

#ifdef TEST_SCENE_SILVER
#define RESOURCE_PATH "test-resources\\silver.gltf"
#else
#define RESOURCE_PATH "test-resources\\sponza\\sponza.gltf"
#endif

using namespace gl;

static int fb_width = 1920, fb_height = 1080;
struct omni_light {
	vec3 position;
	float intensity;
	vec3 color;
	float range;
};

RenderPassInfo NORMAL_PASS{
	.pass = RenderPassType::Normal,
	.view_matrix_location = 1,
	.projection_matrix_location = 2,
	.bind_albedo_texture = true,
	.bind_normal_texture = true,
	.bind_orm_texture = true,
	.bind_object_id = true,
	.frustum_culling = false,
	.render_sky = false,
	.bind_time = std::nullopt
};

RenderPassInfo DEFERRED_PASS{
	.pass = RenderPassType::Normal,
	.view_matrix_location = 6,
	.projection_matrix_location = 7,
	.inverse_view_matrix_location = 5,
	.inverse_projection_matrix_location = 4,
	.bind_albedo_texture = false,
	.bind_normal_texture = false,
	.bind_orm_texture = false,
	.bind_object_id = false,
	.frustum_culling = false,
	.render_sky = true,
	.bind_time = 14
};

RenderPassInfo SHADOW_PASS{
	.pass = RenderPassType::Shadow,
	.view_matrix_location = 1,
	.projection_matrix_location = 2,
	.bind_albedo_texture = true,
	.bind_normal_texture = false,
	.bind_orm_texture = false,
	.bind_object_id = false,
	.frustum_culling = false,
	.render_sky = false,
	.bind_time = std::nullopt
};

class GBuffer {
public:
	Framebuffer fb;
	Renderbuffer rb;
	Texture color;
	Texture normal;
	Texture position;
	Texture orm;
	Texture id;
	GBuffer() :
		color(TextureTarget::Texture2D),
		normal(TextureTarget::Texture2D),
		position(TextureTarget::Texture2D),
		orm(TextureTarget::Texture2D),
		id(TextureTarget::Texture2D) {
	}
};

GBuffer *gbuf = nullptr;

static void createGBuffer() {
	delete gbuf;

	gbuf = new GBuffer();
	ivec2 const resolution(fb_width, fb_height);
	
	gbuf->fb.setLabel("gbuffer");
	gbuf->rb.allocateStorage(resolution, InternalFormat::Depth24Stencil8);
	gbuf->fb.attachRenderbuffer(gbuf->rb, FramebufferAttachment::DepthStencilAttachment);

	gbuf->color.setLabel("unlit texture");
	gbuf->color.allocate(resolution, 1, InternalFormat::Rgba8);
	gbuf->fb.attachTexture(FramebufferAttachment::ColorAttachment0, gbuf->color, 0);

	gbuf->normal.setLabel("normal texture");
	gbuf->normal.allocate(resolution, 1, InternalFormat::Rgba16f);
	gbuf->fb.attachTexture(FramebufferAttachment::ColorAttachment1, gbuf->normal, 0);

	gbuf->position.setLabel("Position texture");
	gbuf->position.allocate(resolution, 1, InternalFormat::Rgba16f);
	gbuf->fb.attachTexture(FramebufferAttachment::ColorAttachment2, gbuf->position, 0);

	gbuf->orm.setLabel("ORM texture");
	gbuf->orm.allocate(resolution, 1, InternalFormat::Rgba8);
	gbuf->fb.attachTexture(FramebufferAttachment::ColorAttachment3, gbuf->orm, 0);

	gbuf->id.setLabel("ID texture");
	gbuf->id.allocate(resolution, 1, InternalFormat::R32ui);
	gbuf->fb.attachTexture(FramebufferAttachment::ColorAttachment4, gbuf->id, 0);

	gbuf->fb.setDrawBuffers({
		ColorBuffer::ColorAttachment0,
		ColorBuffer::ColorAttachment1,
		ColorBuffer::ColorAttachment2,
		ColorBuffer::ColorAttachment3,
		ColorBuffer::ColorAttachment4
	});

	printf("Framebuffer status %s", gl::to_string(gbuf->fb.status()));
	assert(gbuf->fb.status() == gl::FramebufferStatus::FramebufferComplete);
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	fb_width = width;
	fb_height = height;
	createGBuffer();
}

int main(
	[[maybe_unused]] int argc,
	[[maybe_unused]] char* argv[]
) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
#endif
	
	initGraphics();

	FileSystem::instance = _STD make_shared<FileSystem>();
	
	{
		auto path = os::getCurrentDirectory();
		_STD string path_to_test_resource = wstringToString(path);// + ;
		path_to_test_resource.back() = '\\';
		
		path_to_test_resource += RESOURCE_PATH;
		
		window_config config{
			.transparent    = false,
			.resizable      =  true,
			.fullscreen     = false,
			.decorated      =  true,
			.videoMode      = _STD nullopt,
		};
		
		// raii
		SharedPtr<Window> windowPtr = std::make_shared<Window>(
		   ivec2(1920, 1080),
		   "hello",
		   _STD nullopt,
		   _STD nullopt
		);
		Window &mainWindow = *windowPtr;
		mainWindow.hide();
		mainWindow.makeContextCurrent();
		mainWindow.setFramebufferSizeCallback(framebufferSizeCallback);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

		
		ImGui_ImplGlfw_InitForOpenGL(mainWindow.window, true);
		ImGui_ImplOpenGL3_Init();

		glDebugMessageCallback(open_gl_debug_proc, nullptr);
		glViewport(0, 0, fb_width, fb_height);
		
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glEnable(GL_MULTISAMPLE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		//glCullFace(GL_BACK);

		Program defaultProgram("shaders\\default.vert", "shaders\\default.frag");
		Program gBufferWrite("shaders\\g_buffer_write.vert", "shaders\\g_buffer_write.frag");
		Program programFullQuad("shaders\\deferred_shading.vert", "shaders\\deferred_shading.frag");

		mat4
			model = glm::lookAt(vec3(0.0f, -20.0f, -5.0f), vec3(0.0f, 20.0f, -5.0f), vec3(0.0f, 1.0f, 0.0f)),
			view = glm::lookAt(vec3(1.0f,5.0f, -0.0f), vec3(1.0f, 0.0f, -5.0f), vec3(0.0f, 1.0f, 0.0f)),
			proj = glm::perspective(glm::radians(130.0f), 16.0f / 9.0f, 0.1f, 100.0f);

		i32 const
			uModel			= gBufferWrite.uniformLocation("model"),
			uView			= gBufferWrite.uniformLocation("view"),
			uProj			= gBufferWrite.uniformLocation("projection"),
			uBaseColor		= gBufferWrite.uniformLocation("baseColor"),
			uMetalRoughness = gBufferWrite.uniformLocation("metallicRoughness");

		gBufferWrite.setUniform(uModel, model);
		gBufferWrite.setUniform(uView,  view);
		gBufferWrite.setUniform(uProj,  proj);
		
		i32 const uMode = gBufferWrite.uniformLocation("mode");

		// FULL SCREEN QUAD

		u32 fsq_vao, fsq_vbo;
		glCreateVertexArrays(1, &fsq_vao);
		glCreateBuffers(1, &fsq_vbo);
		glBindVertexArray(fsq_vao);

		float fsq_vertices[] = {
			-1.0f, +1.0f, 0.0f, 	0.0f, 1.0f,
			+1.0f, +1.0f, 0.0f, 	1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 	0.0f, 0.0f,
			
			+1.0f, +1.0f, 0.0f, 	1.0f, 1.0f,
			+1.0f, -1.0f, 0.0f, 	1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 	0.0f, 0.0f,
		};
		
		glNamedBufferData(fsq_vbo, sizeof(float) * 30, fsq_vertices, GL_STATIC_DRAW);

		glVertexArrayVertexBuffer(fsq_vao, 0, fsq_vbo, 0, sizeof(float) * 5);

		glVertexArrayAttribFormat(fsq_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(fsq_vao, 0, 0);
		glEnableVertexArrayAttrib(fsq_vao, 0);

		glVertexArrayAttribFormat(fsq_vao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3);
		glVertexArrayAttribBinding(fsq_vao, 1, 0);
		glEnableVertexArrayAttrib(fsq_vao, 1);

		rd::full_screen_quad = fsq_vao;
		// END FULL SCREEN QUAD

		mat4 inverseProjection, inverseView;

		constexpr auto light_count = 64;
		
		if (OmniLightServer::buffer_ == nullptr) {
			OmniLightServer::createBuffer();
			OmniLightServer::resize(light_count);
		}
			
		OmniLightServer::resetCount();
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_real_distribution x_dist(-13.0f, 13.0f);
		std::uniform_real_distribution y_dist(0.5f, 8.0f);
		std::uniform_real_distribution z_dist(-6.0f, 6.0f);
		std::uniform_real_distribution h_dist(0.0f, 1.0f);
		std::uniform_real_distribution s_dist(0.2f, 0.6f);
		std::uniform_real_distribution v_dist(0.4f, 0.8f);
		std::uniform_real_distribution i_dist(0.1f, 100.0f);
		std::uniform_real_distribution r_dist(50.0f, 1000.0f);

		for (size_t i = 0; i < light_count; i++) {
			float hue = h_dist(rng);
			float sat = s_dist(rng);
			float val = v_dist(rng);
			vec3 col = glm::rgbColor(vec3(hue, sat, val));
			
			omni_light light{
				.position = vec3(x_dist(rng), y_dist(rng), z_dist(rng)),
				.intensity = r_dist(rng),
				.color = col,
				.range = r_dist(rng),
			};

			constexpr auto data_size = sizeof(omni_light);
			OmniLightServer::buffer_->update(data_size,
				static_cast<i64>((data_size * i) + sizeof(u32)),
				&light);
		}

		Program depth_write("shaders\\depth_write.vert", "shaders\\depth_write.frag");

		Framebuffer directional_light_fbo;
		Texture depth_texture(
			Texture2DBuilder()
				.resolution(ivec2(2048))
				.internalFormat(InternalFormat::DepthComponent32f)
				.pixelFormat(PixelFormat::DepthComponent)
				.filter(TextureMinFilter::Nearest, TextureMagFilter::Nearest)
		);
		directional_light_fbo.attachTexture(FramebufferAttachment::DepthAttachment, depth_texture);

		glNamedFramebufferDrawBuffer(directional_light_fbo.framebuffer_object_, GL_NONE);
		glNamedFramebufferReadBuffer(directional_light_fbo.framebuffer_object_, GL_NONE);
		std::cout << gl::to_string(directional_light_fbo.status()) << '\n';

		constexpr i32 uDwModel = 0;
		constexpr i32 uDwLight = 1;
		constexpr i32 uDwProjection = 2;
		constexpr i32 uDwAlbedoTexture = 3;

		vec3 directionalLightOrigin(-21.7048587799072f, 43.414340972900390f, -6.149883747100830f);
		mat4 directionalLight = glm::lookAt(
		  vec3(-21.7048587799072f, 43.414340972900390f, -6.149883747100830f),
		vec3(6.477884292602539f, 1.0242811441421509f, -0.759415328502655f),
		  vec3(0.0f, 1.0f, 0.0f)
		);

		float lightFov = 50.0f, lightNear = 0.01f, lightFar = 100.0f;
		float lightSize = 1.0f;
		bool lightOrtho = false;

		vec3 light_dir = glm::normalize(vec3(-21.7048587799072f, 43.414340972900390f, -6.149883747100830f) - vec3(6.477884292602539f, 1.0242811441421509f, -0.759415328502655f));

		mat4 directionalProj = glm::ortho(3.0f, 3.0f, 3.0f, 3.0f, 0.1f, 2048.0f);

		auto tree = _STD make_shared<SceneTree>(windowPtr);
		{
			auto s = simdjson::padded_string::load(path_to_test_resource).value();
			auto gltf_test_data = gltf::parse(path_to_test_resource,_STD move(s));
			uid root = gltf::createEntityFromGltf(tree, gltf_test_data);
			tree->setRoot(root);
		}
		
		mainWindow.show();
		mainWindow.setSceneTree(tree);

		Result<uid> camera_id = tree->createEntity();
		tree->entity(0)->addChild(tree->entity(camera_id.value()));
		Transform const &xform = tree->entity(camera_id.value())->component<Transform>();
		EditorCamera3D &camera = tree->entity(camera_id.value())->component<EditorCamera3D>();
		camera.setAspectRatio(16.0f / 9.0f);
		camera.setFov(65.0f);
		camera.setFarPlane(512.0f);
		camera.setNearPlane(0.01f);
		Environment const &env = tree->entity(1)->component<Environment>();
		DirectionalLight const &dl = tree->entity(127)->component<DirectionalLight>();
		
		//vertexArray.enableAttribute(0);
		f64 lastTimeStamp = glfwGetTime();
		f64 delta = 0.0;
		bool viewTheLightSource = false;
		while (!mainWindow.shouldClose()) {
			camera.setAspectRatio(static_cast<float>(fb_width) / static_cast<float>(fb_height));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			f32 const time = static_cast<f32>(glfwGetTime());
			tree->initiateFrame(delta);

#ifndef TEST_SCENE_0
			model = mat4(1.0f);
			model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
			view  = 
				glm::lookAt(
				tree->entity(2)->component<Transform>().translation,
				tree->entity(1)->component<Transform>().translation,
				vec3(0.0f, 1.0f, 0.0f)
			);//glm::vec3((glm::cos(time * .80f) * 10.0f), 20.0f * glm::tan(glm::cos(time * 8.0) * glm::sin(time * 8.0)), (glm::sin(time * 8.0f) * 10.0f)), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			
			proj  = glm::perspective(65.0f, static_cast<float>(fb_width) / static_cast<float>(fb_height), 0.01f, 128.0f);

			inverseView = glm::inverse(view);
			inverseProjection = glm::inverse(proj);
			
			auto light = tree->entity(27)->component<Transform>().translation;
			glUniform3fv(9, 1, glm::value_ptr(light));

			FileSystem::instance->process();
			gBufferWrite.use();
			
			depth_write.integrityCheck();
			gBufferWrite.integrityCheck();
			programFullQuad.integrityCheck();
			
#else
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			view  = glm::lookAt(glm::vec3(glm::cos(time * 2.0f) * 2.0f, glm::sin(time * 4.0f) * 2.0f, glm::sin(time * 2.0f) * 2.0f + 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));//glm::vec3((glm::cos(time * .80f) * 10.0f), 20.0f * glm::tan(glm::cos(time * 8.0) * glm::sin(time * 8.0)), (glm::sin(time * 8.0f) * 10.0f)), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			proj  = glm::perspective(40.0f, 16.0f / 9.0f, 0.1f, 300.0f);
#endif
			//assert(uModel == 0);
			gBufferWrite.setUniform(uModel, model);
			gBufferWrite.setUniform(uView, view);
			gBufferWrite.setUniform(uProj, proj);

			gBufferWrite.setUniform(uBaseColor, 0);
			gBufferWrite.setUniform(uMetalRoughness, 1);

			if (glfwGetKey(mainWindow.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(mainWindow.window, GLFW_TRUE);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F1))
				gBufferWrite.setUniform(uMode, 1);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F2))
				gBufferWrite.setUniform(uMode, 2);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F3))
				gBufferWrite.setUniform(uMode, 3);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F4))
				gBufferWrite.setUniform(uMode, 4);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F5))
				gBufferWrite.setUniform(uMode, 5);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F6))
				gBufferWrite.setUniform(uMode, 6);
			if (glfwGetKey(mainWindow.window, 296))
				gBufferWrite.setUniform(uMode, 7);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F8))
				gBufferWrite.setUniform(uMode, 8);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F9))
				gBufferWrite.setUniform(uMode, 9);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F10))
				gBufferWrite.setUniform(uMode, 10);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F11))
				gBufferWrite.setUniform(uMode, 11);
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F12))
				gBufferWrite.setUniform(uMode, 12);
			if (Input::justPressed(mainWindow, KEY_X))
				viewTheLightSource = !viewTheLightSource;
			if (!gbuf)
				createGBuffer();
			
			Framebuffer &fbo = gbuf->fb;

			glBindFramebuffer(GL_FRAMEBUFFER, fbo.framebuffer_object_);
			glViewport(0, 0, fb_width, fb_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			NORMAL_PASS.viewport = windowPtr->viewport();
			tree->initiateRenderSetup(NORMAL_PASS);

			if (viewTheLightSource) {
				gBufferWrite.setUniform(uView, directionalLight);
				gBufferWrite.setUniform(uProj, directionalProj);
			}

			NORMAL_PASS.camera = camera.makeFrustum();
			tree->initiateDraw(NORMAL_PASS);

			
// #if 1
			glBindFramebuffer(GL_FRAMEBUFFER, directional_light_fbo.framebuffer_object_);
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, 2048, 2048);
			depth_write.use();
			directionalLight = glm::lookAt(
				tree->entity(127)->component<Transform>().translation,
				tree->entity(2)->component<Transform>().translation,
				vec3(0.0f, 1.0f, 0.0f)
			);
			directionalProj = lightOrtho
				? glm::ortho(-lightSize, lightSize, -lightSize, lightSize, lightNear, lightFar)
				: glm::perspective(lightFov, 1.0f, lightNear, lightFar);
			depth_write.setUniform(uDwLight, directionalLight);
			depth_write.setUniform(uDwProjection, directionalProj);
			
			SHADOW_PASS.viewport = ivec4(0, 0, 2048, 2048);
			
			tree->initiateRenderSetup(SHADOW_PASS);
			tree->initiateDraw(SHADOW_PASS);
			glViewport(0, 0, fb_width, fb_height);
// #endif
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			#if 0 
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo.framebuffer_object_);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBlitFramebuffer(0, 0, fb_width, fb_height, 0, 0, fb_width, fb_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			#endif
			
			tree->renderExtraPasses();
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, fb_width, fb_height);

			DEFERRED_PASS.viewport = windowPtr->viewport();
			
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			tree->drawEditors();

			if (ImGui::Begin("light debug :)")) {
				ImGui::Checkbox("Orthographic?", &lightOrtho);
				
				if (lightOrtho)
					ImGui::DragFloat("Size", &lightSize, 0.1f, 0.1f, 5000.0f);
				else
					ImGui::DragFloat("Field of View", &lightFov, 0.1f, 1.0f, 179.0f);
				ImGui::DragFloat("Near-Z Clipping Plane", &lightNear, 0.01f, 0.0f, 128.0f);
				ImGui::DragFloat("Far-Z Clipping Plane", &lightFar, 0.1f, 1.0f, 16384.0f);
			}
			ImGui::End();
			
			programFullQuad.use();
			gbuf->color.bindTextureUnit(1);
			programFullQuad.setUniform(0, 1);

			gbuf->position.bindTextureUnit(2);
			programFullQuad.setUniform(1, 2);

			gbuf->normal.bindTextureUnit(3);
			programFullQuad.setUniform(2, 3);

			gbuf->orm.bindTextureUnit(4);
			programFullQuad.setUniform(3, 4);

			gbuf->id.bindTextureUnit(5);
			programFullQuad.setUniform(20, 5);

			programFullQuad.setUniform(16, directionalLight);
			programFullQuad.setUniform(17, directionalProj);
			programFullQuad.setUniform(18, vec2(lightNear, lightFar));
			depth_texture.bindTextureUnit(5);
			programFullQuad.setUniform(19, 5);

			
			programFullQuad.setUniform(11, light);
			
			tree->initiateRenderSetup(DEFERRED_PASS);

			camera.refreshMatrices();
			
			programFullQuad.setUniform(4, camera.inverseProjectionMatrix());
			programFullQuad.setUniform(5, camera.inverseViewMatrix());
			programFullQuad.setUniform(6, camera.projectionMatrix());
			programFullQuad.setUniform(7, camera.viewMatrix());
			
			OmniLightServer::buffer_->bindBufferBase(BufferTargetARB::ShaderStorageBuffer,1);
			glViewport(0, 0, fb_width, fb_height);
			glBindVertexArray(fsq_vao);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			glUniform1d(14, time);
			//env.renderSky(rd::full_screen_quad, light_dir, view);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			Input::process(mainWindow);
			glfwPollEvents();
			mainWindow.swapBuffers();
			
			delta = glfwGetTime() - lastTimeStamp;
			lastTimeStamp = glfwGetTime();
		}
		glDeleteVertexArrays(1, &fsq_vao);
		glDeleteBuffers(1, &fsq_vbo);
	}

	

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete gbuf;
	
	terminateGraphics();
	
	return 0;
}
