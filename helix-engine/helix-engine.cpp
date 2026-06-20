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
#include "engine/engine.h"
#include "engine/filesystem.hpp"
#include "engine/Input.h"
#include "gpu/compositor.h"
#include "gpu/geometry_buffer.hpp"

#include "gpu/graphics.hpp"
#include "gpu/mesh.hpp"
#include "gpu/gltf.h"
#include "gpu/lighting.hpp"
#include "gpu/placeholders.hpp"
#include "gpu/png.hpp"
#include "gpu/render_server.h"
#include "gpu/texture.h"
#include "gpu/voxelizer.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "stb/stb_image.h"

#ifdef TEST_SCENE_SILVER
#define RESOURCE_PATH "test-resources\\silver.gltf"
#else
#define RESOURCE_PATH "test-resources\\sponza\\NewSponza_Main_glTF_003.gltf"
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
	.model_matrix_location = 0,
	.view_matrix_location = 1,
	.projection_matrix_location = 2,
	.bind_albedo_texture = true,
	.bind_normal_texture = true,
	.bind_orm_texture = true,
	.bind_object_id = true,
	.frustum_culling = false,
	.render_sky = false,
	.cull = true,
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
	.cull = true,
	.bind_time = std::nullopt,
	.csm = {
		.bind_buffer = true,
		.buffer_binding = 2,
		.bind_texture_unit = 7,
		.light_direction_location = 22,
		.far_plane_location = 23,
		.world_position_location = 24
	}
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
	.cull = true,
	.bind_time = std::nullopt
};

static GBuffer *gbuf = nullptr;
static Compositor *compositor = nullptr;

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	fb_width = width;
	fb_height = height;
	gbuf->changeResolution(ivec2(width, height));
	compositor->resize(ivec2(width, height));
}

static void R_WriteToGBuffer(Program &gBufferWrite, SharedPtr<SceneTree> const &tree, Camera3D const &camera, SharedPtr<Window> windowPtr) {
	glEnable(GL_MULTISAMPLE);
	gBufferWrite.setUniform("baseColor", 0);
	gBufferWrite.setUniform("metallicRoughness", 1);
	gBufferWrite.setUniform("normalTexture", 2);
	gBufferWrite.setUniform("u_emissiveTexture", 3);

	NORMAL_PASS.model_matrix_location = gBufferWrite.uniformLocation("model");
	NORMAL_PASS.view_matrix_location = gBufferWrite.uniformLocation("view");
	NORMAL_PASS.projection_matrix_location = gBufferWrite.uniformLocation("projection");
	NORMAL_PASS.inverse_model_matrix_location = gBufferWrite.uniformLocation("inverse_model");
	NORMAL_PASS.inverse_view_matrix_location = gBufferWrite.uniformLocation("inverse_view");
	NORMAL_PASS.inverse_projection_matrix_location = gBufferWrite.uniformLocation("inverse_projection");

	NORMAL_PASS.shader_program = &gBufferWrite;
			
	NORMAL_PASS.material_bridge = {
		.diffuse_texture_unit = 0,
		.diffuse_texture_is_used = gBufferWrite.uniformLocation("u_hasBaseColorTexture"),
		.diffuse_color_modulation = gBufferWrite.uniformLocation("u_baseColorFactor"),
		.orm_texture_unit = 1,
		.orm_texture_is_used = gBufferWrite.uniformLocation("u_hasMetallicRoughnessTexture"),
		.normal_texture_unit = 2,
		.normal_texture_is_used = gBufferWrite.uniformLocation("u_hasNormalTexture"),
		.emissive_texture_unit = 3,
		.emissive_texture_is_used = gBufferWrite.uniformLocation("u_hasEmissiveTexture"),
		.emissive_color_modulation = gBufferWrite.uniformLocation("u_emissiveBias")
	};
	if (!gbuf)
		gbuf = new GBuffer(ivec2(fb_width, fb_height));
			
	gbuf->bind();
	glViewport(0, 0, fb_width, fb_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	NORMAL_PASS.viewport = windowPtr->viewport();
	tree->initiateRenderSetup(NORMAL_PASS);

	NORMAL_PASS.camera = camera.makeFrustum();
	tree->initiateDraw(NORMAL_PASS);
	glDisable(GL_MULTISAMPLE);
}

static void R_Voxelize(Voxelizer const &voxelizer, SharedPtr<SceneTree> const &tree) {
	glDisable(GL_CULL_FACE);
	auto &voxelPass = voxelizer.renderPassInfo();
	voxelizer.useProgram();
	voxelizer.program_->setUniform("u_renderAxis", 0);
	tree->initiateDraw(voxelPass);
	voxelizer.program_->setUniform("u_renderAxis", 1);
	tree->initiateDraw(voxelPass);
	voxelizer.program_->setUniform("u_renderAxis", 2);
	tree->initiateDraw(voxelPass);
}

static void R_DeferredLighting(Program &programFullQuad, Voxelizer const &voxelizer, SharedPtr<SceneTree> const &tree, Camera3D &camera, SharedPtr<Window> windowPtr) {
	programFullQuad.use();
	gbuf->color().bindTextureUnit(0);
	gbuf->position().bindTextureUnit(1);
	gbuf->normal().bindTextureUnit(2);
	gbuf->orm().bindTextureUnit(3);
	gbuf->id().bindTextureUnit(4);
	gbuf->emissive().bindTextureUnit(5);
	
	programFullQuad.setUniform("texEmissive", 6);
	programFullQuad.setUniform("csmTexture", 7);

	DEFERRED_PASS.shader_program = &programFullQuad;
	tree->initiateRenderSetup(DEFERRED_PASS);

	camera.refreshMatrices();
			
	programFullQuad.setUniform(4, camera.inverseProjectionMatrix());
	programFullQuad.setUniform(5, camera.inverseViewMatrix());
	programFullQuad.setUniform(6, camera.projectionMatrix());
	programFullQuad.setUniform(7, camera.viewMatrix());

	compositor->beginDraw();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
			
	OmniLightServer::buffer_->bindBufferBase(BufferTargetARB::ShaderStorageBuffer, 1);

	LightingSystem::singleton()->prerender();

	glViewport(0, 0, fb_width, fb_height);gpu_check;
	glBindVertexArray(rd::full_screen_quad); gpu_check;
	glDrawArrays(GL_TRIANGLES, 0, 6); gpu_check;

	voxelizer.world_grid_->bindTextureUnit(6);
	voxelizer.data_buffer_->bindBufferBase(BufferTargetARB::ShaderStorageBuffer, 0);

	compositor->endDraw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);gpu_check;
}

static void R_ScreenSpaceReflections(Program const &ssr, Camera3D const &camera) {
	ssr.use();
	compositor->bindRenderOutputToUnit(0);
	compositor->ssrTexture().bindImage(4, InternalFormat::Rgba8, BufferAccessARB::WriteOnly);
	gbuf->position().bindTextureUnit(1);
	gbuf->normal().bindTextureUnit(2);

	ssr.setUniform(4, camera.inverseProjectionMatrix());
	ssr.setUniform(5, camera.inverseViewMatrix());
	ssr.setUniform(6, camera.projectionMatrix());
	ssr.setUniform(7, camera.viewMatrix());

	ssr.setUniform("u_halfResolution", compositor->halfSizeSSR());

	ssr.dispatchCompute(
		// 16x16
		((compositor->halfSizeSSR() ? fb_width/2 : fb_width) + 15) / 16,
		((compositor->halfSizeSSR() ? fb_height/2 : fb_height) + 15) / 16,
		1
	);
}

static void R_DrawPp() {
	
}

static void R_PreProcessAndEditors(SharedPtr<SceneTree> const &tree, SharedPtr<Window> windowPtr) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			
	tree->renderExtraPasses();
			
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, fb_width, fb_height);

	DEFERRED_PASS.viewport = windowPtr->viewport();
			
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	tree->drawEditors();
}
int main(
	[[maybe_unused]] int argc,
	[[maybe_unused]] char* argv[]
) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
#endif

	Engine::singleton()->markAsMainThread();
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

		LightingSystem::singleton(); //< Allocation
		AsyncTextureBank::singleton(); // LET IT ALLOCATE ON MAIN THREAD
		
		auto tree = _STD make_shared<SceneTree>(windowPtr);

		auto load_model_async = [path_to_test_resource] {
			auto s = simdjson::padded_string::load(path_to_test_resource).value();
			auto gltf_test_data = gltf::parse(path_to_test_resource,_STD move(s));
			return gltf_test_data;
		};

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
		}

		{
			Vec<ubyte_t> missing_texture_pixels = {
				  0, 0,   0, 255, 0, 255,
				255, 0, 255,   0, 0,   0
			};
			rd::missing_texture = new Texture(Texture2DBuilder()
				.resolution(ivec2(2))
				.filter(TextureMinFilter::Nearest, TextureMagFilter::Nearest)
				.wrapMode(TextureWrapMode::Repeat)
				.internalFormat(InternalFormat::Rgba8)
				.pixelFormat(PixelFormat::Rgba)
				.pixelType(PixelType::UnsignedByte)
				.imageData(missing_texture_pixels.data())
			);
		}

		Program depth_write("shaders\\depth_write.vert", "shaders\\depth_write.frag");
		depth_write.setLabel("Depth Write Program");

		Framebuffer directional_light_fbo;
		Texture depth_texture(
			Texture2DBuilder()
				.resolution(ivec2(2048))
				.internalFormat(InternalFormat::DepthComponent32f)
				.pixelFormat(PixelFormat::DepthComponent)
				.compareFunc(CompareFunction::Less)
				.compareMode(TextureCompareMode::CompareRefToTexture)
				.filter(TextureMinFilter::Nearest, TextureMagFilter::Nearest)
		);
		directional_light_fbo.attachTexture(FramebufferAttachment::DepthAttachment, depth_texture);

		glNamedFramebufferDrawBuffer(directional_light_fbo.framebuffer_object_, GL_NONE);
		glNamedFramebufferReadBuffer(directional_light_fbo.framebuffer_object_, GL_NONE);
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

		auto gltf_test_data = load_model_async();
		uid root = gltf::createEntityFromGltf(tree, gltf_test_data);
		tree->setRoot(root);
		
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
		Result<uid> environment_id = tree->createEntity();
		Environment const &env = tree->entity(environment_id.value())->component<Environment>();

		tree->entity(0)->addChild(tree->entity(environment_id.value()));
		
		Result<uid> dlight_id = tree->createEntity();
		DirectionalLight const &dl = tree->entity(dlight_id.value())->component<DirectionalLight>();
		tree->entity(0)->addChild(tree->entity(dlight_id.value()));

		compositor = new Compositor();
		
		//vertexArray.enableAttribute(0);
		f64 lastTimeStamp = glfwGetTime();
		f64 delta = 0.0;
		bool viewTheLightSource = false;

		Voxelizer voxelizer;
		voxelizer.setGridMinimum(vec3(-16.f, -4.0f, -16.0f));
		voxelizer.setGridMaximum(vec3(16.f, 28.0f, 16.0f));

		gpu_check;

		float *lights = OmniLightServer::beginWrite();
		if (lights != nullptr) {
			size_t light = 0;
			tree->visitComponent([&light, lights](OmniLight const *ol) {
				std::memcpy(&lights[light * 8], &ol->data_, sizeof(OmniLight::OmniLightStorage));
				light++;
			}, 0);
			OmniLightServer::endWrite();
		}

		OmniLightServer::bindBuffer(4);

		Program ssr("shaders\\screenspace_reflections.comp");
		Program drawTexture("shaders\\deferred_shading.vert", "shaders\\texture_to_screen.frag");


		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		while (!mainWindow.shouldClose()) {

			Engine::singleton()->workLazyTasks();
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			camera.setAspectRatio(static_cast<float>(fb_width) / static_cast<float>(fb_height));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			f32 const time = static_cast<f32>(glfwGetTime());
			LightingSystem::singleton()->prerender();
			tree->initiateFrame(delta);
			LightingSystem::singleton()->prerender();
			
			model = mat4(1.0f);
			model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
			view  = 
				glm::lookAt(
				tree->entity(2)->component<Transform>().translation,
				tree->entity(1)->component<Transform>().translation,
				vec3(0.0f, 1.0f, 0.0f)
			);
			//glm::vec3((glm::cos(time * .80f) * 10.0f), 20.0f * glm::tan(glm::cos(time * 8.0) * glm::sin(time * 8.0)), (glm::sin(time * 8.0f) * 10.0f)), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			
			proj  = glm::perspective(65.0f, static_cast<float>(fb_width) / static_cast<float>(fb_height), 0.01f, 128.0f);

			inverseView = glm::inverse(view);
			inverseProjection = glm::inverse(proj);
			
			// auto light = tree->entity(27)->component<Transform>().translation;
			// glUniform3fv(9, 1, glm::value_ptr(light));

			FileSystem::instance->process();
			gBufferWrite.use();
			
			depth_write.integrityCheck();
			gBufferWrite.integrityCheck();
			programFullQuad.integrityCheck();
			voxelizer.integrityCheck();
			ssr.integrityCheck();
			drawTexture.integrityCheck();
			LightingSystem::singleton()->pointShadowProgram().integrityCheck();

			R_WriteToGBuffer(gBufferWrite, tree, camera, windowPtr);
			R_Voxelize(voxelizer, tree);
			R_PreProcessAndEditors(tree, windowPtr);
			compositor->editor();
			R_DeferredLighting(programFullQuad, voxelizer, tree, camera, windowPtr);
			R_ScreenSpaceReflections(ssr, camera);

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			glBindVertexArray(rd::full_screen_quad);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, fb_width, fb_height);

			drawTexture.use();
			compositor->bindRenderOutputToUnit(0);
			compositor->ssrTexture().bindTextureUnit(1);
			
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			Input::process(mainWindow);
			glfwPollEvents();
			try {
				mainWindow.swapBuffers();
			}
			catch (std::exception const &e) {
				std::cout << "Error during buffer swap: " << e.what() << std::endl;
				break;
			}
			delta = glfwGetTime() - lastTimeStamp;
			lastTimeStamp = glfwGetTime();

			if (Input::justPressed(mainWindow, KEY_ESCAPE)) {
				glfwSetWindowShouldClose(mainWindow.window, true);
			}
			
			RenderServer::singleton().prune();
			Engine::singleton()->incrementFrameCount();
		}
		glDeleteVertexArrays(1, &fsq_vao);
		glDeleteBuffers(1, &fsq_vbo);
	}

	delete rd::missing_texture;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete gbuf;
	delete compositor;

	// RenderServer::singleton().shutdown();
	
	terminateGraphics();
	
	return 0;
}
