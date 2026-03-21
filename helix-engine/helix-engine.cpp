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

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	fb_width = width;
	fb_height = height;
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

		CProgram programObject;
		CShader vertexStage(gl::ShaderType::VertexShader), fragmentStage(gl::ShaderType::FragmentShader);

		{
			_STD ifstream vertexShaderFile("shaders/default.vert");
			vertexShaderFile.seekg(0, _STD ios::end);
			_STD size_t size = vertexShaderFile.tellg();
			_STD string vertexShaderContent(size + 1, '\0');
			vertexShaderFile.seekg(0, _STD ios::beg);
			vertexShaderFile.read(vertexShaderContent.data(), static_cast<_STD streamsize>(size));
			vertexStage.setSource(vertexShaderContent, "shaders\\default.vert");
		}

		{
			_STD ifstream fragmentShaderFile("shaders/default.frag");
			fragmentShaderFile.seekg(0, _STD ios::end);
			_STD size_t size = fragmentShaderFile.tellg();
			_STD string fragmentShaderContent(size + 1, '\0');
			fragmentShaderFile.seekg(0, _STD ios::beg);
			fragmentShaderFile.read(fragmentShaderContent.data(), static_cast<_STD streamsize>(size));
			fragmentStage.setSource(fragmentShaderContent, "shaders\\default.frag");
		}

		vertexStage.compile();
		if (!vertexStage.compileStatus()) _UNLIKELY {
			_STD string const infoLogVert = vertexStage.infoLog();
			_STD cout << infoLogVert << "\n\n\n";
			__debugbreak();
			return 100;
		}
		
		fragmentStage.compile();
		if (!fragmentStage.compileStatus()) _UNLIKELY {
			_STD string const infoLogFrag = fragmentStage.infoLog();
			_STD cout << infoLogFrag << '\n';
			__debugbreak();
			return 100;
		}

		programObject.attach(vertexStage);
		programObject.attach(fragmentStage);
		programObject.link();
		programObject.use();

		glm::mat4
			model = glm::lookAt(glm::vec3(0.0f, -20.0f, -5.0f), glm::vec3(0.0f, 20.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
			view = glm::lookAt(glm::vec3(1.0f,5.0f, -0.0f), glm::vec3(1.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
			proj = glm::perspective(glm::radians(130.0f), 16.0f / 9.0f, 0.1f, 100.0f);

		i32 const
			uMvp = programObject.uniformLocation("modelViewProjection"),
			uModel = programObject.uniformLocation("model"),
			uView = programObject.uniformLocation("view"),
			uProj = programObject.uniformLocation("projection"),
			uBaseColor = programObject.uniformLocation("baseColor"),
			uMetalRoughness = programObject.uniformLocation("metallicRoughness");
		programObject.setUniform(uMvp, model * view * proj);
		programObject.setUniform(uModel, model);
		programObject.setUniform(uView, view);
		programObject.setUniform(uProj, proj);

		i32 const uColor = programObject.uniformLocation("color");
		programObject.setUniform(uColor, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		i32 const uMode = programObject.uniformLocation("mode");

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
			auto light = tree->entity(27)->component<CTransform>().translation;
			glViewport(0, 0, fb_width, fb_height);
			glUniform3fv(9, 1, glm::value_ptr(light));

			CFileSystemMonitor::instance->process();
			programObject.integrityCheck();
			
#else
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			view  = glm::lookAt(glm::vec3(glm::cos(time * 2.0f) * 2.0f, glm::sin(time * 4.0f) * 2.0f, glm::sin(time * 2.0f) * 2.0f + 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));//glm::vec3((glm::cos(time * .80f) * 10.0f), 20.0f * glm::tan(glm::cos(time * 8.0) * glm::sin(time * 8.0)), (glm::sin(time * 8.0f) * 10.0f)), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			proj  = glm::perspective(40.0f, 16.0f / 9.0f, 0.1f, 300.0f);
#endif
			//assert(uModel == 0);
			programObject.setUniform(uModel, model);
			programObject.setUniform(uView, view);
			programObject.setUniform(uProj, proj);
			//mesh.
			
			//mesh.textures_.back()->bindTextureUnit(0);
			programObject.setUniform(uBaseColor, 0);
			programObject.setUniform(uMetalRoughness, 1);

			if (glfwGetKey(mainWindow.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(mainWindow.window, GLFW_TRUE);
			}

			if (glfwGetKey(mainWindow.window, GLFW_KEY_F1)) {
				programObject.setUniform(uMode, 1);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F2)) {
				programObject.setUniform(uMode, 2);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F3)) {
				programObject.setUniform(uMode, 3);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F4)) {
				programObject.setUniform(uMode, 4);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F5)) {
				programObject.setUniform(uMode, 5);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F6)) {
				programObject.setUniform(uMode, 6);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F7)) {
				programObject.setUniform(uMode, 7);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F8)) {
				programObject.setUniform(uMode, 8);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F9)) {
				programObject.setUniform(uMode, 9);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F10)) {
				programObject.setUniform(uMode, 10);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F11)) {
				programObject.setUniform(uMode, 11);
			}
			if (glfwGetKey(mainWindow.window, GLFW_KEY_F12)) {
				programObject.setUniform(uMode, 12);
			}
			
			//mesh.drawAllSubMeshes();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			tree->initiateFrame();
			tree->drawEditors();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			glfwPollEvents();
			mainWindow.swapBuffers();
		}
	}


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	terminateGraphics();

	
	return 0;
}
