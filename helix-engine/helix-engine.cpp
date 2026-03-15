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

#include "os.hpp"
#include "util.hpp"

#include "ecs/ecs.hpp"
#include "ecs/transform.h"

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

int main(
	[[maybe_unused]] int argc,
	[[maybe_unused]] char* argv[]
) {
	initGraphics();
	
	{
		os::initDirectoryWatcher();
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

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
		
		ImGui_ImplGlfw_InitForOpenGL(mainWindow.window, true);
		ImGui_ImplOpenGL3_Init();

		glDebugMessageCallback(open_gl_debug_proc, nullptr);
		glViewport(0, 0, 1920, 1080);

		auto tree = _STD make_shared<CSceneTree>();
		{
			auto s = simdjson::padded_string::load(path_to_test_resource).value();
			auto gltf_test_data = gltf::parse(path_to_test_resource,_STD move(s));
			uid root = gltf::createEntityFromGltf(tree, gltf_test_data);
			tree->setRoot(root);
		}
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
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
			vertexStage.setSource(vertexShaderContent);
		}

		{
			_STD ifstream fragmentShaderFile("shaders/default.frag");
			fragmentShaderFile.seekg(0, _STD ios::end);
			_STD size_t size = fragmentShaderFile.tellg();
			_STD string fragmentShaderContent(size + 1, '\0');
			fragmentShaderFile.seekg(0, _STD ios::beg);
			fragmentShaderFile.read(fragmentShaderContent.data(), static_cast<_STD streamsize>(size));
			fragmentStage.setSource(fragmentShaderContent);
		}

		vertexStage.compile();
		vertexStage.compileStatus();

		_STD string const infoLogVert = vertexStage.infoLog();
		_STD cout << infoLogVert << "\n\n\n";
		
		fragmentStage.compile();
		fragmentStage.compileStatus();
		
		_STD string const infoLogFrag = fragmentStage.infoLog();
		_STD cout << infoLogFrag << '\n';

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
				glm::vec3(
					glm::cos(time * .80f) * 2.0f,
					2.0f,
					glm::sin(time * .80f) * 2.0f
				),
				tree->entity(1)->component<CTransform>().translation,
				glm::vec3(0.0f, 1.0f, 0.0f)
			);//glm::vec3((glm::cos(time * .80f) * 10.0f), 20.0f * glm::tan(glm::cos(time * 8.0) * glm::sin(time * 8.0)), (glm::sin(time * 8.0f) * 10.0f)), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			
			proj  = glm::perspective(80.0f, 16.0f / 9.0f, 0.01f, 128.0f);
			
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
