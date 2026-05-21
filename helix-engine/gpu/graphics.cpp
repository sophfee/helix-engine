// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
// ReSharper disable CppCStyleCast
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <Windows.h>

#include "buffer.h"
#include "os.hpp"
#include "texture.h"
#include "util.hpp"
#include "engine/filesystem.hpp"
#include "engine/Input.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "khr/ktx.h"

void initGraphics() {
	HELIX_ASSUME(glfwInit() == GLFW_TRUE, "GLFW3 failed to initialize");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	
	GLFWwindow *window = glfwCreateWindow(1, 1, "Context Loader", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	HELIX_ASSUME(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0);
	// HELIX_ASSUME(ktxLoadOpenGL(glfwGetProcAddress) == KTX_SUCCESS, "Failed to load KTX library!");

	glfwDestroyWindow(window);
}

void terminateGraphics() {
	glfwTerminate();
}

// #define SKIP_ERR_CHECK
// #define  GPU_ERRORS_ARE_EXCEPTIONS

#ifdef GPU_ERRORS_ARE_EXCEPTIONS
#define GL_ThrowError(msg) throw std::runtime_error(msg)
#else
#define GL_ThrowError(code, line, file, msg) printf("OpenGL Error [%u]: %s (in \"%s\" at line %llu)\n", code, msg, file, line)
#endif

static size_t errors_checked = 0;

bool gpu::check(char const *where, _STD size_t const line) {
	//printf("[%s:%llu] Checking for OpenGL errors... (%llu checks)\n", where, line, errors_checked++);
#ifndef SKIP_ERR_CHECK
	//printf("[%s:%llu] Checking for OpenGL errors...", where, line);
	gl::enum_t err = glGetError();
	while (err != 0) {
		//printf("[%s:%llu] OpenGL has encountered an error: \"%s\"", where, line, gl::to_pretty_string(static_cast<gl::ErrorCode>(err)));

		switch (err) {
			case GL_NO_ERROR:
				return true;
			case GL_INVALID_ENUM:
				GL_ThrowError(err, line, where, "Invalid Enum");
				break;
			case GL_INVALID_OPERATION:
				GL_ThrowError(err, line, where, "Invalid Operation");
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				GL_ThrowError(err, line, where, "Invalid Framebuffer Operation");
				break;
			case GL_OUT_OF_MEMORY:
				GL_ThrowError(err, line, where, "Out of Memory");
				break;
			case GL_STACK_OVERFLOW:
				GL_ThrowError(err, line, where, "Stack Overflow");
				break;
			case GL_STACK_UNDERFLOW:
				GL_ThrowError(err, line, where, "Stack Underflow");
				break;
			default:
				GL_ThrowError(err, line, where, "Unknown GL Error");
				break;
		}
		err = glGetError();
	}
#endif
	return true;
}

// Window Impl


Window::Window(
	glm::ivec2 const &p_startingSize,
	_STD optional<_STD string> const &p_windowTitle,
	_STD optional<_STD reference_wrapper<Window>> const &p_sharedWindow,
	_STD optional<window_config> const &p_config
) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	bool bMakeFullscreen = false;
	if (p_config.has_value()) {
		auto &config = p_config.value();
		bMakeFullscreen = config.fullscreen;
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, config.transparent ? GLFW_TRUE : GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);
		glfwWindowHint(GLFW_DECORATED, config.decorated ? GLFW_TRUE : GLFW_FALSE);
	}

	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	
	window = glfwCreateWindow(p_startingSize.x, p_startingSize.y,
		p_windowTitle.has_value() ? p_windowTitle.value().c_str() : "New Window", nullptr,
		p_sharedWindow.has_value() ? p_sharedWindow.value().get().window : nullptr);

	glfwMakeContextCurrent(window);
	glfwSetErrorCallback([](int error_code, const char *description) {
		printf("GLFW Error [%d]: %s\n", error_code, description);
	});

	assert(window);
	glfwSwapInterval(0);
	if (bMakeFullscreen) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		glm::ivec2 size;
		glfwGetMonitorPhysicalSize(monitor, &size.x, &size.y);
	}

	Input::installCallbacks(*this);
}

Window::Window() = default;

Window::~Window() {
	glfwDestroyWindow(window);
}


glm::ivec2 Window::getSize() const {
	glm::ivec2 size;
	glfwGetWindowSize(window, &size.x, &size.y);
	return size;
}

void Window::setSize(glm::ivec2 const &size) const {
	glfwSetWindowSize(window, size.x, size.y);
}

ivec4 Window::viewport() const {
	ivec2 vp(0.0f);
	glfwGetFramebufferSize(window, &vp.x, &vp.y);
	return ivec4(0, 0, vp.x, vp.y);
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(window);
}

void Window::setSceneTree(SharedPtr<SceneTree> const &scene_tree) {
	scene_tree_ = scene_tree;
}

SharedPtr<SceneTree> const & Window::sceneTree() const {
	return scene_tree_;
}

void Window::hide() const {
	glfwHideWindow(window);
}

void Window::show() const {
	glfwShowWindow(window);
}

void Window::setVisible(bool const visible) const {
	if (visible) show(); else hide();
}

bool Window::visible() const {
	return glfwGetWindowAttrib(window, GLFW_VISIBLE) == GLFW_TRUE;
}

void Window::setFramebufferSizeCallback(GLFWframebuffersizefun const fun) const {
	glfwSetFramebufferSizeCallback(window, fun);
}

void Window::makeContextCurrent() const {
	glfwMakeContextCurrent(window);
}
void Window::swapBuffers() const {
	glfwSwapBuffers(window);
}

void Window::dispose() {
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool Window::disposed() const {
	return window == nullptr;
}

// Program

Program::Program() : program_object_(glCreateProgram()) {
}

Program::Program(std::string_view compute) : program_object_(glCreateProgram()) {
	auto computeShader = std::make_unique<Shader>(gl::ShaderType::ComputeShader, compute);
	attach(std::move(computeShader));
	link();
}

Program::Program(std::string_view const vert, std::string_view const frag) : program_object_(glCreateProgram()) {
	auto vertexStage   = std::make_unique<Shader>(gl::ShaderType::VertexShader,   vert);
	auto fragmentStage = std::make_unique<Shader>(gl::ShaderType::FragmentShader, frag);
	attach(std::move(vertexStage));
	attach(std::move(fragmentStage));
	link();
}

Program::Program(std::string_view vert, std::string_view geom, std::string_view frag) : program_object_(glCreateProgram()) {
	auto vertexStage    =  std::make_unique<Shader>(gl::ShaderType::VertexShader,   vert);
	auto geometryStage  =  std::make_unique<Shader>(gl::ShaderType::GeometryShader, geom);
	auto fragmentStage  =  std::make_unique<Shader>(gl::ShaderType::FragmentShader, frag);
	vertexStage->assertStatus();
	geometryStage->assertStatus();
	fragmentStage->assertStatus();
	attach(std::move(vertexStage));
	attach(std::move(geometryStage));
	attach(std::move(fragmentStage));
	link();
}

Program::~Program() {
	glDeleteProgram(program_object_);
}

void Program::attach(Shader &p_shader) {
	glAttachShader(program_object_, p_shader.shader_object_); gpu_check;
	shaders_.push_back(_STD ref(p_shader));
}

void Program::attach(Box<Shader> p_shader) {
	glAttachShader(program_object_, p_shader->shader_object_); gpu_check;
	p_shader->assertStatus();
	owned_shaders_.push_back(std::move(p_shader));
}

void Program::setLabel(_STD string_view const p_label) const {
	glObjectLabel(
		GL_PROGRAM,
		program_object_,
		static_cast<gl::sizei_t>(p_label.size()),
		p_label.data()
	);
	gpu_check;
}

void Program::link() const {
	glLinkProgram(program_object_); gpu_check;
}

std::string Program::linkLog() const {
	gl::int32_t size;
	glGetProgramiv(program_object_, static_cast<GLenum>(gl::ProgramPropertyARB::InfoLogLength), &size);
	auto const log = new gl::char_t[size];
	_STD memset(log, '\0', size);
	glGetProgramInfoLog(program_object_, size, nullptr, log);
	gpu_check;
	_STD string logStr(log);
	delete[] log;
	return logStr;
}

void Program::use() const {
	if (program_in_use_ == program_object_)
		return;
	glUseProgram(program_object_);
	gpu_check;
	program_in_use_ = program_object_;
}

#ifdef PROGRAM_INTEGRITY_CHECK_DEBUG
#define IntegrityCheckDebug(message) printf("Program integrity check: %s\n", message)
#else
#define IntegrityCheckDebug(...)
#endif

void Program::integrityCheck() {
	bool any_failed_checks = false;
	for (std::reference_wrapper<Shader> &shader : shaders_) {
		bool const check = shader.get().integrityCheck();
		any_failed_checks |= check;
	}
	for (UniquePtr<Shader> const &shader : owned_shaders_) {
		bool const check = shader->integrityCheck();
		any_failed_checks |= check;
	}
	if (any_failed_checks) {
		IntegrityCheckDebug("Shader has been updated! Recompiling!");
		bool const currently_in_use = program_in_use_ == program_object_;
		for (std::reference_wrapper<Shader> &shader : shaders_) {
			glAttachShader(program_object_, shader.get().shader_object_);
			gpu_check;
		}
		for (UniquePtr<Shader> const &shader : owned_shaders_) {
			glAttachShader(program_object_, shader->shader_object_);
			gpu_check;
		}
		glLinkProgram(program_object_); gpu_check;
	}
}

void Program::dispatchCompute(u32 const num_groups_x, u32 const num_groups_y, u32 const num_groups_z) const {
	if (!inUse()) use();
	glDispatchCompute(num_groups_x, num_groups_y, num_groups_z); gpu_check;
}

void Program::dispatchComputeIndirect(GLintptr const indirect_offset) const {
	if (!inUse()) use();
	glDispatchComputeIndirect(indirect_offset); gpu_check;
}

bool Program::inUse() const {
	return program_in_use_ == program_object_;
}

i32 Program::uniformLocation(_STD string const &p_name) const {
	return glGetUniformLocation(program_object_, p_name.c_str());
}

void Program::setUniform(i32 const uniform, glm::mat4 const &p_matrix, bool const transposed) const { glProgramUniformMatrix4fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }
void Program::setUniform(i32 const uniform, glm::mat3 const &p_matrix, bool const transposed) const { glProgramUniformMatrix3fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }
void Program::setUniform(i32 const uniform, glm::mat2 const &p_matrix, bool const transposed) const { glProgramUniformMatrix2fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }

void Program::setUniform(_STD string const &uniform, mat4 const &p_matrix, bool const transposed) const { setUniform(uniformLocation(uniform), p_matrix, transposed); }
void Program::setUniform(_STD string const &uniform, mat3 const &p_matrix, bool const transposed) const { setUniform(uniformLocation(uniform), p_matrix, transposed); }
void Program::setUniform(_STD string const &uniform, glm::mat2 const &p_matrix, bool const transposed) const { setUniform(uniformLocation(uniform), p_matrix, transposed); }

void Program::setUniform(i32 const uniform, _STD vector<glm::mat4> const &p_matrices, bool const transposed) const { glProgramUniformMatrix4fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }
void Program::setUniform(i32 const uniform, _STD vector<glm::mat3> const &p_matrices, bool const transposed) const { glProgramUniformMatrix3fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }
void Program::setUniform(i32 const uniform, _STD vector<glm::mat2> const &p_matrices, bool const transposed) const { glProgramUniformMatrix2fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }

void Program::setUniform(_STD string const &uniform, std::vector<mat4> const &p_matrices, bool const transposed) const { setUniform(uniformLocation(uniform), p_matrices, transposed); }
void Program::setUniform(_STD string const &uniform, std::vector<mat3> const &p_matrices, bool const transposed) const { setUniform(uniformLocation(uniform), p_matrices, transposed); }
void Program::setUniform(_STD string const &uniform, std::vector<glm::mat2> const &p_matrices, bool const transposed) const { setUniform(uniformLocation(uniform), p_matrices, transposed); }

void Program::setUniform(i32 const uniform, glm::vec4 const &p_vector) const { glProgramUniform4f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void Program::setUniform(i32 const uniform, glm::vec3 const &p_vector) const { glProgramUniform3f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void Program::setUniform(i32 const uniform, glm::vec2 const &p_vector) const { glProgramUniform2f(program_object_, uniform, p_vector.x, p_vector.y); }

void Program::setUniform(_STD string const &uniform, vec4 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }
void Program::setUniform(_STD string const &uniform, vec3 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }
void Program::setUniform(_STD string const &uniform, vec2 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }

void Program::setUniform(i32 const uniform, _STD vector<glm::vec4> const &p_vectors) const { glProgramUniform4fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void Program::setUniform(i32 const uniform, _STD vector<glm::vec3> const &p_vectors) const { glProgramUniform3fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void Program::setUniform(i32 const uniform, _STD vector<glm::vec2> const &p_vectors) const { glProgramUniform2fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void Program::setUniform(_STD string const &uniform, std::vector<vec4> const &p_vectors) const { setUniform(uniformLocation(uniform), p_vectors); }
void Program::setUniform(_STD string const &uniform, std::vector<vec3> const &p_vectors) const { setUniform(uniformLocation(uniform), p_vectors); }
void Program::setUniform(_STD string const &uniform, std::vector<vec2> const &p_vectors) const { setUniform(uniformLocation(uniform), p_vectors); }

void Program::setUniform(i32 const uniform, glm::ivec4 const &p_vector) const { glProgramUniform4i(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void Program::setUniform(i32 const uniform, glm::ivec3 const &p_vector) const { glProgramUniform3i(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void Program::setUniform(i32 const uniform, glm::ivec2 const &p_vector) const { glProgramUniform2i(program_object_, uniform, p_vector.x, p_vector.y); }
void Program::setUniform(_STD string const &uniform, ivec4 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }
void Program::setUniform(_STD string const &uniform, ivec3 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }
void Program::setUniform(_STD string const &uniform, ivec2 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }

void Program::setUniform(i32 const uniform, _STD vector<glm::ivec4> const &p_vectors) const { glProgramUniform4iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void Program::setUniform(i32 const uniform, _STD vector<glm::ivec3> const &p_vectors) const { glProgramUniform3iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void Program::setUniform(i32 const uniform, _STD vector<glm::ivec2> const &p_vectors) const { glProgramUniform2iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void Program::setUniform(_STD string const &uniform, std::vector<ivec4> const &p_vectors) const { setUniform(uniformLocation(uniform), p_vectors); }
void Program::setUniform(_STD string const &uniform, std::vector<ivec3> const &p_vectors) const { setUniform(uniformLocation(uniform), p_vectors); }
void Program::setUniform(_STD string const &uniform, std::vector<ivec2> const &p_vectors) const { setUniform(uniformLocation(uniform), p_vectors); }

void Program::setUniform(i32 const uniform, glm::bvec4 const &p_vector) const { glProgramUniform4f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void Program::setUniform(i32 const uniform, glm::bvec3 const &p_vector) const { glProgramUniform3f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void Program::setUniform(i32 const uniform, glm::bvec2 const &p_vector) const { glProgramUniform2f(program_object_, uniform, p_vector.x, p_vector.y); }
void Program::setUniform(_STD string const &uniform, glm::bvec4 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }
void Program::setUniform(_STD string const &uniform, glm::bvec3 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }
void Program::setUniform(_STD string const &uniform, glm::bvec2 const &p_vector) const { setUniform(uniformLocation(uniform), p_vector); }

//void Program::setUniform(i32 uniform, _STD vector<glm::bvec4> const &p_vectors) const { glProgramUniform4iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), static_cast<const GLint *>(glm::value_ptr(p_vectors[0]))); }
//void Program::setUniform(i32 uniform, _STD vector<glm::bvec3> const &p_vectors) const { glProgramUniform3iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
//void Program::setUniform(i32 uniform, _STD vector<glm::bvec2> const &p_vectors) const { glProgramUniform2iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }


void Program::setUniform(i32 const uniform, i32 const value) const { glProgramUniform1i(program_object_, uniform, value); }
void Program::setUniform(i32 const uniform, u32 const value) const { glProgramUniform1ui(program_object_, uniform, value); }
void Program::setUniform(i32 const uniform, i64 const value) const { glProgramUniform1i64ARB(program_object_, uniform, value); }
void Program::setUniform(i32 const uniform, u64 const value) const { glProgramUniform1ui64ARB(program_object_, uniform, value); }
void Program::setUniform(i32 const uniform, f32 const value) const { glProgramUniform1f(program_object_, uniform, value); }
void Program::setUniform(_STD string const &uniform, i32 const value) const { setUniform(uniformLocation(uniform), value); }
void Program::setUniform(_STD string const &uniform, u32 const value) const { setUniform(uniformLocation(uniform), value); }
void Program::setUniform(_STD string const &uniform, i64 const value) const { setUniform(uniformLocation(uniform), value); }
void Program::setUniform(_STD string const &uniform, u64 const value) const { setUniform(uniformLocation(uniform), value); }
void Program::setUniform(_STD string const &uniform, f32 const value) const { setUniform(uniformLocation(uniform), value); }
void Program::dispose() { glDeleteProgram(program_object_); }
bool Program::disposed() const { return glIsProgram(program_object_) == GL_FALSE; }

// Shader

Shader::Shader(gl::ShaderType p_shaderType) :
	shader_object_(glCreateShader(static_cast<GLenum>(p_shaderType))),
	shader_type_(p_shaderType) {
}

Shader::Shader(gl::ShaderType p_shaderType, std::string_view const p_fileName) :
	shader_object_(glCreateShader(static_cast<GLenum>(p_shaderType))),
	shader_type_(p_shaderType) {
	setFileSource(p_fileName);
	compile();
	assertStatus();
}

Shader::Shader(_STD string const &p_source, gl::ShaderType p_shaderType) :
	shader_object_(glCreateShader(static_cast<GLenum>(p_shaderType))),
	shader_type_(p_shaderType) {
	setSource(p_source);
}

Shader::~Shader() {
	glDeleteShader(shader_object_);
}

void Shader::setLabel(_STD string_view const p_label) const {
	glObjectLabel(GL_SHADER, shader_object_,
		static_cast<GLsizei>(p_label.size()),
		p_label.data()); gpu_check;
}

void Shader::compile() const {
	glCompileShader(shader_object_);gpu_check;
}

void Shader::setSource(std::string_view const p_source, std::string_view const p_file_name) {
	char const *sources = p_source.data();
	GLsizei const length = static_cast<GLsizei>(p_source.size());
	glShaderSource(shader_object_, 1, &sources, &length); gpu_check;
	if (p_file_name.empty()) return;
	source_file_ = _STD string(p_file_name.data(), p_file_name.size());
	_STD function const lambda = [this](EFileAction const action) {
		if (action == EFileAction::Modified)
			recompile();
	};
	FileSystem::instance->createListener(p_file_name, lambda);
}

void Shader::setFileSource(std::string_view const p_file_name) {
	_STD string file_name(p_file_name.data(), p_file_name.size());
	_STD ifstream source_stream(file_name);
	source_stream.seekg(0, _STD ios::end);
	_STD size_t source_size = source_stream.tellg();
	_STD string source_content(source_size + 1, '\0');
	source_stream.seekg(0, _STD ios::beg);
	source_stream.read(source_content.data(), static_cast<_STD streamsize>(source_size));
	setSource(source_content, p_file_name);
}
void Shader::assertStatus() const {
	if (!compileStatus()) _UNLIKELY {
		_STD string const info_log = infoLog();
		throw std::runtime_error(info_log);
	}
}

void Shader::recompile() {
	_STD ifstream shader_file(source_file_);
	shader_file.seekg(0, _STD ios::end);
	_STD size_t const shader_file_size = shader_file.tellg();
	_STD string source_content(shader_file_size + 1, '\0');
	shader_file.seekg(0, _STD ios::beg);
	shader_file.read(source_content.data(), static_cast<_STD streamsize>(shader_file_size));
	shader_file.close();

	char const *source = source_content.data();
	gl::sizei_t const length = static_cast<GLsizei>(source_content.size());
	glShaderSource(shader_object_, 1, &source, &length); gpu_check;
	glCompileShader(shader_object_); gpu_check;

	needs_relinking_ = true;
	assertStatus();
}

_STD string Shader::source() const {
	gl::sizei_t size;
	glGetShaderSource(shader_object_, 0, &size, nullptr); gpu_check;
	auto const log = new gl::char_t[size];
	_STD memset(log, 0, size);
	glGetShaderSource(shader_object_, size, &size, log); gpu_check;
	_STD string logStr(log);
	delete[] log;
	return logStr;
}

_STD string Shader::infoLog() const {
	gl::int32_t size;
	glGetShaderiv(shader_object_, static_cast<GLenum>(gl::ShaderParameterName::InfoLogLength), &size);
	auto const log = new gl::char_t[size];
	_STD memset(log, '\0', size);
	glGetShaderInfoLog(shader_object_, size, nullptr, log);
	gpu_check;
	_STD string logStr(log);
	delete[] log;
	return logStr;
}

gl::ShaderType Shader::type() const {
	return shader_type_;
}

i32 Shader::compileStatus() const {
	i32 value;
	glGetShaderiv(shader_object_, GL_COMPILE_STATUS, &value);
	return value;
}

bool Shader::integrityCheck() {
	if (needs_relinking_) {
		needs_relinking_ = false;
		return true;
	}
	return false;
}

void Shader::dispose() {
	glDeleteShader(shader_object_);
}
bool Shader::disposed() const {
	return glIsShader(shader_object_) == GL_FALSE;
}

VertexArray::VertexArray(): IDisposable(), vertex_array_object_(0), is_deleted_(false) {
	glCreateVertexArrays(1, &vertex_array_object_);
	gpuDebugf("Vertex Array #%u has been born", vertex_array_object_);
}

VertexArray::~VertexArray() {
	if (!is_deleted_) {
		gpuDebugf("Vertex Array #%u destroyed", vertex_array_object_);
		glDeleteVertexArrays(1, &vertex_array_object_);
	}
}
void VertexArray::bind() const {
	//if (bound_object_ == vertex_array_object_)
	//	return;
	glBindVertexArray(vertex_array_object_);gpu_check;
	bound_object_ = vertex_array_object_;
}

void VertexArray::unbind() const {
	if (bound_object_ == vertex_array_object_) {
		glBindVertexArray(0);
		bound_object_ = 0;
	}
}

void VertexArray::setLabel(_STD string_view const p_label) const {
	glObjectLabel(GL_VERTEX_ARRAY, vertex_array_object_, static_cast<GLsizei>(p_label.size()), p_label.data());
	gpu_check;
}

void VertexArray::enableAttribute(u32 const p_bindingindex) const {
	glEnableVertexArrayAttrib(vertex_array_object_, p_bindingindex);
	gpu_check;
}
void VertexArray::setAttribute(VertexAttribute_t const &p_attrib) const {
	switch (GLenum const attrib_type = componentTypeToGL(p_attrib.type)) {
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:
			glVertexArrayAttribIFormat(
				vertex_array_object_,
				p_attrib.index,
				p_attrib.size,
				attrib_type,
				p_attrib.offset
			);
			break;
		default:
			glVertexArrayAttribFormat(
				vertex_array_object_,
				p_attrib.index,
				p_attrib.size,
				attrib_type,
				p_attrib.normalized,
				p_attrib.offset
			);
			break;
	} gpu_check;
	glVertexArrayAttribBinding(vertex_array_object_, p_attrib.index, p_attrib.binding); gpu_check;
	glEnableVertexArrayAttrib(vertex_array_object_, p_attrib.index); gpu_check;
}

void VertexArray::setVertexBuffer(u32 const p_bindingindex, Buffer const &buffer, i32 const p_stride, i64 const p_offset) const {
	glVertexArrayVertexBuffer(vertex_array_object_, p_bindingindex, buffer.buffer_object_, p_offset, p_stride);
	gpu_check;
}

void VertexArray::setElementBuffer(Buffer const &buffer) const {
	glVertexArrayElementBuffer(vertex_array_object_, buffer.buffer_object_);
	gpu_check;
}
bool VertexArray::bound() const {
	return bound_object_ == vertex_array_object_;
}

void VertexArray::drawArrays(gl::PrimitiveType prim, i32 const first, i32 const count) const {
	bind();
	glDrawArrays(static_cast<GLenum>(prim), first, count);
	gpu_check;
}
void VertexArray::drawArraysInstanced(gl::PrimitiveType prim, i32 const first, i32 const count, i32 const instances) const {
	bind();
	glDrawArraysInstanced(static_cast<GLenum>(prim), first, count, instances);
	gpu_check;
}
void VertexArray::drawElements(gl::PrimitiveType prim, gl::DrawElementsType elem, i32 const count) const {
	bind();
	glDrawElements(static_cast<GLenum>(prim), count, static_cast<GLenum>(elem), nullptr);
	gpu_check;
}
void VertexArray::draw() const {
	bind();
	glDrawElements(
		static_cast<GLenum>(primitive_type),
		static_cast<GLsizei>(elements_count),
		static_cast<GLenum>(draw_elements_type),
		(void const *)offset_of_elements
	);
	gpu_check;
}
void VertexArray::dispose() {
	glDeleteVertexArrays(1, &vertex_array_object_);
	gpu_check;
}
bool VertexArray::disposed() const {
	return glIsVertexArray(vertex_array_object_) == GL_FALSE;
}

void open_gl_debug_proc(GLenum source, GLenum type, GLuint const id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam) {
	_STD string const source_str = gl::toPrettyString(static_cast<gl::DebugSource>(source));
	_STD string const type_str = gl::toPrettyString(static_cast<gl::DebugType>(type));

	if (id == 0) __debugbreak();

	_STD cout << "[" << source_str << "] " << type_str << " #" << id << ": " << message << '\n';
}