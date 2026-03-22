// ReSharper disable CppCStyleCast
#ifdef _DEBUG
#define STBI_FAILURE_USERMSG
#endif
#define STB_IMAGE_IMPLEMENTATION

#include <chrono>
#include <iostream>
#include <thread>
#include <bit>

#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "os.hpp"
#include "util.hpp"

#include "ecs/ecs.hpp"
#include "ecs/transform.h"
#include "engine/filesystem.hpp"

#include "gpu/graphics.hpp"
#include "gpu/mesh.hpp"
#include "gpu/gltf.h"
#include "gpu/png.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "stb/stb_image.h"

#ifdef TEST_SCENE_SILVER
#define RESOURCE_PATH "test-resources\\silver.gltf"
#else
#define RESOURCE_PATH "test-resources\\sponza\\sponza.gltf"
#endif

static int fb_width = 1920, fb_height = 1080;

class GBuffer {
public:
	CFramebuffer fb;
	CRenderbuffer rb;
	CTexture color;
	CTexture normal;
	CTexture position;
	CTexture orm;
	GBuffer() :
		color(gl::TextureTarget::Texture2D),
		normal(gl::TextureTarget::Texture2D),
		position(gl::TextureTarget::Texture2D),
		orm(gl::TextureTarget::Texture2D) {
	}
};

GBuffer *gbuf = nullptr;

static void createGBuffer() {
	delete gbuf;

	gbuf = new GBuffer();
	glm::ivec2 const resolution(fb_width, fb_height);
	
	gbuf->fb.setLabel("gbuffer");
	gbuf->rb.allocateStorage(resolution, gl::InternalFormat::Depth24Stencil8);
	gbuf->fb.attachRenderbuffer(gbuf->rb, gl::FramebufferAttachment::DepthStencilAttachment);

	gbuf->color.setLabel("unlit texture");
	gbuf->color.allocate(resolution, 1, gl::InternalFormat::Rgba8);
	gbuf->fb.attachTexture(gl::ColorBuffer::ColorAttachment0, gbuf->color, 0);

	gbuf->normal.setLabel("normal texture");
	gbuf->normal.allocate(resolution, 1, gl::InternalFormat::Rgba16f);
	gbuf->fb.attachTexture(gl::ColorBuffer::ColorAttachment1, gbuf->normal, 0);

	gbuf->position.setLabel("Position texture");
	gbuf->position.allocate(resolution, 1, gl::InternalFormat::Rgba16f);
	gbuf->fb.attachTexture(gl::ColorBuffer::ColorAttachment2, gbuf->position, 0);

	gbuf->orm.setLabel("ORM texture");
	gbuf->orm.allocate(resolution, 1, gl::InternalFormat::Rgba8);
	gbuf->fb.attachTexture(gl::ColorBuffer::ColorAttachment3, gbuf->orm, 0);

	gbuf->fb.setDrawBuffers({
		gl::ColorBuffer::ColorAttachment0,
		gl::ColorBuffer::ColorAttachment1,
		gl::ColorBuffer::ColorAttachment2,
		gl::ColorBuffer::ColorAttachment3
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

	CFileSystemMonitor::instance = _STD make_shared<CFileSystemMonitor>();
	
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
		CWindow mainWindow(
		   glm::ivec2(1920, 1080),
		   "hello",
		   _STD nullopt,
		   _STD nullopt
		);
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
		
		auto tree = _STD make_shared<CSceneTree>();
		{
			auto s = simdjson::padded_string::load(path_to_test_resource).value();
			auto gltf_test_data = gltf::parse(path_to_test_resource,_STD move(s));
			uid root = gltf::createEntityFromGltf(tree, gltf_test_data);
			tree->setRoot(root);
		}
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_MULTISAMPLE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		glCullFace(GL_BACK);

		CProgram gBufferWrite;
		CShader vertexStage(gl::ShaderType::VertexShader, "shaders\\default.vert"),
				fragmentStage(gl::ShaderType::FragmentShader, "shaders\\default.frag");

		gBufferWrite.attach(vertexStage);
		gBufferWrite.attach(fragmentStage);
		gBufferWrite.link();
		gBufferWrite.use();
		
		CProgram defaultProgram;
		CShader gBufferWriteVert(gl::ShaderType::VertexShader, "shaders\\g_buffer_write.vert"),
				gBufferWriteFrag(gl::ShaderType::FragmentShader, "shaders\\g_buffer_write.frag");
		
		
		defaultProgram.attach(gBufferWriteVert);
		defaultProgram.attach(gBufferWriteFrag);
		defaultProgram.link();
		defaultProgram.use();

		CProgram programFullQuad;
		CShader fullQuadVert(gl::ShaderType::VertexShader,   "shaders\\deferred_shading.vert"),
				fullQuadFrag(gl::ShaderType::FragmentShader, "shaders\\deferred_shading.frag");
		
		programFullQuad.attach(fullQuadVert);
		programFullQuad.attach(fullQuadFrag);
		programFullQuad.link();
		programFullQuad.use();

		glm::mat4
			model = glm::lookAt(glm::vec3(0.0f, -20.0f, -5.0f), glm::vec3(0.0f, 20.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
			view = glm::lookAt(glm::vec3(1.0f,5.0f, -0.0f), glm::vec3(1.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
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
		
		// END FULL SCREEN QUAD

		mainWindow.show();

		//vertexArray.enableAttribute(0);
		while (!mainWindow.shouldClose()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			

			f32 const time = static_cast<f32>(glfwGetTime());

#ifndef TEST_SCENE_0
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			view  = 
				glm::lookAt(
				tree->entity(2)->component<CTransform>().translation,
				tree->entity(1)->component<CTransform>().translation,
				glm::vec3(0.0f, 1.0f, 0.0f)
			);//glm::vec3((glm::cos(time * .80f) * 10.0f), 20.0f * glm::tan(glm::cos(time * 8.0) * glm::sin(time * 8.0)), (glm::sin(time * 8.0f) * 10.0f)), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			
			proj  = glm::perspective(80.0f, static_cast<float>(fb_width) / static_cast<float>(fb_height), 0.01f, 128.0f);
			auto light = tree->entity(1)->component<CTransform>().translation;
			glViewport(0, 0, fb_width, fb_height);
			glUniform3fv(9, 1, glm::value_ptr(light));

			CFileSystemMonitor::instance->process();
			gBufferWrite.use();
			
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
			//mesh.
			
			//mesh.textures_.back()->bindTextureUnit(0);
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

			//if (!gbuf)
			//	createGBuffer();

			//CFramebuffer &fbo = gbuf->fb;

			//glBindFramebuffer(GL_FRAMEBUFFER, fbo.framebuffer_object_);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			tree->initiateFrame();
			
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			tree->drawEditors();

			#if 0 // deferred just keep disabled for now
			programFullQuad.use();
			gbuf->color.bindTextureUnit(1);
			programFullQuad.setUniform(0, 1);

			gbuf->position.bindTextureUnit(2);
			programFullQuad.setUniform(1, 2);

			gbuf->normal.bindTextureUnit(3);
			programFullQuad.setUniform(2, 3);

			gbuf->orm.bindTextureUnit(4);
			programFullQuad.setUniform(3, 4);

			glBindVertexArray(fsq_vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			#endif
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			glfwPollEvents();
			mainWindow.swapBuffers();
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
