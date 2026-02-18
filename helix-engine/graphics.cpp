#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics.hpp"

#include <chrono>
#include <iostream>

#include "glad/glad.h"
#include "glfw/glfw3.h"

void initGraphics() {
	assert(glfwInit() == GLFW_TRUE, "GLFW3 failed to initialize");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	
	GLFWwindow *window = glfwCreateWindow(1, 1, "Context Loader", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	assert(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0);

	glfwDestroyWindow(window);
}

void terminateGraphics() {
	glfwTerminate();
}

bool gpu::check(char const *where, std::size_t const line) {
	gl::enum_t err = glGetError();
	while (err != static_cast<gl::enum_t>(gl::ErrorCode::NoError)) {
		printf("[%s:%llu] OpenGL has encountered an error: \"%s\"", where, line, gl::to_pretty_string(static_cast<gl::ErrorCode>(err)));
		err = glGetError();
	}
	return true;
}

// CWindow Impl

CWindow::CWindow(
	glm::ivec2 const &p_startingSize,
	std::optional<std::string> const &p_windowTitle,
	std::optional<std::reference_wrapper<CWindow>> const &p_sharedWindow,
	std::optional<window_config> const &p_config
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

	// glfwWindowHint(GLFW_SAMPLES, 8);
	
	window = glfwCreateWindow(p_startingSize.x, p_startingSize.y,
		p_windowTitle.has_value() ? p_windowTitle.value().c_str() : "New Window", nullptr,
		p_sharedWindow.has_value() ? p_sharedWindow.value().get().window : nullptr);

	assert(window);
	glfwSwapInterval(0);

	if (bMakeFullscreen) {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		glm::ivec2 size;
		glfwGetMonitorPhysicalSize(monitor, &size.x, &size.y);
	}
}

CWindow::CWindow() = default;

CWindow::~CWindow() {
	glfwDestroyWindow(window);
}


glm::ivec2 CWindow::getSize() const {
	glm::ivec2 size;
	glfwGetWindowSize(window, &size.x, &size.y);
	return size;
}

void CWindow::setSize(glm::ivec2 const &size) const {
	glfwSetWindowSize(window, size.x, size.y);
}

void CWindow::makeContextCurrent() const {
	glfwMakeContextCurrent(window);
}
void CWindow::swapBuffers() const {
	glfwSwapBuffers(window);
}

CProgram::CProgram() : program_object_(glCreateProgram()) {
}

CProgram::~CProgram() {
	glDeleteProgram(program_object_);
}


// CProgram

void CProgram::attach(CShader const &p_shaderObject) const {
	glAttachShader(program_object_, p_shaderObject.shader_object_);
}

void CProgram::link() const {
	glLinkProgram(program_object_);
}
void CProgram::use() const {
	if (program_in_use_ == program_object_)
		return;
	glUseProgram(program_object_);
	program_in_use_ = program_object_;
}

bool CProgram::inUse() const {
	return program_in_use_ == program_object_;
}

i32 CProgram::uniformLocation(std::string const &p_name) const {
	return glGetUniformLocation(program_object_, p_name.c_str());
}

void CProgram::setUniform(i32 const uniform, glm::mat4 const &p_matrix, bool const transposed) const { glProgramUniformMatrix4fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }
void CProgram::setUniform(i32 const uniform, glm::mat3 const &p_matrix, bool const transposed) const { glProgramUniformMatrix3fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }
void CProgram::setUniform(i32 const uniform, glm::mat2 const &p_matrix, bool const transposed) const { glProgramUniformMatrix2fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }

void CProgram::setUniform(i32 const uniform, std::vector<glm::mat4> const &p_matrices, bool const transposed) const { glProgramUniformMatrix4fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }
void CProgram::setUniform(i32 const uniform, std::vector<glm::mat3> const &p_matrices, bool const transposed) const { glProgramUniformMatrix3fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }
void CProgram::setUniform(i32 const uniform, std::vector<glm::mat2> const &p_matrices, bool const transposed) const { glProgramUniformMatrix2fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }

void CProgram::setUniform(i32 const uniform, glm::vec4 const &p_vector) const { glProgramUniform4f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void CProgram::setUniform(i32 const uniform, glm::vec3 const &p_vector) const { glProgramUniform3f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void CProgram::setUniform(i32 const uniform, glm::vec2 const &p_vector) const { glProgramUniform2f(program_object_, uniform, p_vector.x, p_vector.y); }

void CProgram::setUniform(i32 const uniform, std::vector<glm::vec4> const &p_vectors) const { glProgramUniform4fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void CProgram::setUniform(i32 const uniform, std::vector<glm::vec3> const &p_vectors) const { glProgramUniform3fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void CProgram::setUniform(i32 const uniform, std::vector<glm::vec2> const &p_vectors) const { glProgramUniform2fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }

void CProgram::setUniform(i32 const uniform, glm::ivec4 const &p_vector) const { glProgramUniform4i(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void CProgram::setUniform(i32 const uniform, glm::ivec3 const &p_vector) const { glProgramUniform3i(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void CProgram::setUniform(i32 const uniform, glm::ivec2 const &p_vector) const { glProgramUniform2i(program_object_, uniform, p_vector.x, p_vector.y); }

void CProgram::setUniform(i32 const uniform, std::vector<glm::ivec4> const &p_vectors) const { glProgramUniform4iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void CProgram::setUniform(i32 const uniform, std::vector<glm::ivec3> const &p_vectors) const { glProgramUniform3iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void CProgram::setUniform(i32 const uniform, std::vector<glm::ivec2> const &p_vectors) const { glProgramUniform2iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }

void CProgram::setUniform(i32 const uniform, glm::bvec4 const &p_vector) const { glProgramUniform4f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void CProgram::setUniform(i32 const uniform, glm::bvec3 const &p_vector) const { glProgramUniform3f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void CProgram::setUniform(i32 const uniform, glm::bvec2 const &p_vector) const { glProgramUniform2f(program_object_, uniform, p_vector.x, p_vector.y); }

//void CProgram::setUniform(i32 uniform, std::vector<glm::bvec4> const &p_vectors) const { glProgramUniform4iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), static_cast<const GLint *>(glm::value_ptr(p_vectors[0]))); }
//void CProgram::setUniform(i32 uniform, std::vector<glm::bvec3> const &p_vectors) const { glProgramUniform3iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
//void CProgram::setUniform(i32 uniform, std::vector<glm::bvec2> const &p_vectors) const { glProgramUniform2iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }


void CProgram::setUniform(i32 const uniform, i32 const value) const { glProgramUniform1i(program_object_, uniform, value); }
void CProgram::setUniform(i32 const uniform, u32 const value) const { glProgramUniform1ui(program_object_, uniform, value); }
void CProgram::setUniform(i32 const uniform, i64 const value) const { glProgramUniform1i64ARB(program_object_, uniform, value); }
void CProgram::setUniform(i32 const uniform, u64 const value) const { glProgramUniform1ui64ARB(program_object_, uniform, value); }

// CShader

CShader::CShader(gl::ShaderType p_shaderType) : shader_object_(glCreateShader(static_cast<GLenum>(p_shaderType))), shader_type_(p_shaderType) {
}

CShader::CShader(std::string const &p_source, gl::ShaderType p_shaderType) {
	setSource(p_source);
}

CShader::~CShader() {
	glDeleteShader(shader_object_);
}

void CShader::compile() const {
	glCompileShader(shader_object_);
}

void CShader::setSource(std::string const &p_source) const {
	char const *sources = p_source.c_str();
	glShaderSource(shader_object_, 1, &sources, nullptr);
}

std::string CShader::source() const {
	gl::sizei_t size;
	glGetShaderSource(shader_object_, 0, &size, nullptr);
	auto const log = new gl::char_t[size];
	std::memset(log, 0, size);
	glGetShaderSource(shader_object_, size, &size, log);
	std::string logStr(log);
	delete[] log;
	return logStr;
}

std::string CShader::infoLog() const {
	gl::int32_t size;
	glGetShaderiv(shader_object_, static_cast<GLenum>(gl::ShaderParameterName::InfoLogLength), &size);
	std::cout << size << '\n';
	auto const log = new gl::char_t[size];
	std::memset(log, '\0', size);
	glGetShaderInfoLog(shader_object_, size, nullptr, log);
	std::cout << log << '\n';
	std::string logStr(log);
	delete[] log;
	return logStr;
}

gl::ShaderType CShader::type() const {
	return shader_type_;
}

i32 CShader::compileStatus() const {
	i32 value;
	glGetShaderiv(shader_object_, GL_COMPILE_STATUS, &value);
	std::cout << "compile status: " << value << '\n';
	return value;
}

// CTexture

CTexture::CTexture(gl::TextureTarget p_textureTarget) {
	glCreateTextures(static_cast<GLenum>(p_textureTarget), 1, &texture_object_);
}

CTexture::~CTexture() {
	glDeleteTextures(1, &texture_object_);
}

i32 CTexture::intParam(gl::GetTextureParameter p_param) const {
	i32 iValue;
	glGetTextureParameteriv(texture_object_, static_cast<GLenum>(p_param), &iValue);
	return iValue;
}

void CTexture::setIntParam(gl::GetTextureParameter p_param, i32 const p_intParameter) const {
	glTextureParameteri(texture_object_, static_cast<GLenum>(p_param), p_intParameter);
}

u32 CTexture::uintParam(gl::GetTextureParameter p_param) const {
	u32 uiValue;
	glGetTextureParameterIuiv(texture_object_, static_cast<GLenum>(p_param), &uiValue);
	return uiValue;
}

void CTexture::setUIntParam(gl::GetTextureParameter p_param, u32 const p_uintParameter) const {
	glTextureParameterIuiv(texture_object_, static_cast<GLenum>(p_param), &p_uintParameter);
}

std::vector<i32> CTexture::intVecParam(gl::GetTextureParameter p_param) const {
	return {};
}

void CTexture::setIntVecParam(gl::GetTextureParameter p_param, std::vector<i32> const &p_vecParameter) const {
}
void CTexture::bindTextureUnit(u32 unit) const {
	glBindTextureUnit(unit, texture_object_);
}

void CTexture::allocate(glm::ivec2 const &size, i32 levels, gl::InternalFormat internalFormat) const {
	glTextureStorage2D(texture_object_, levels, static_cast<GLenum>(internalFormat), size.x, size.y);
}

void CTexture::setImage2D(void const *data, i32 const level, glm::ivec2 const &offset, glm::ivec2 const &size, gl::PixelFormat format, gl::PixelType type) const {
	glTextureSubImage2D(
		texture_object_,
		level,
		offset.x, offset.y,
		size.x, size.y,
		static_cast<GLenum>(format),
		static_cast<GLenum>(type),
		data
	);
}


void CVertexArray::enableAttribute(u32 const p_bindingindex) const {
	glEnableVertexArrayAttrib(vertex_array_object_, p_bindingindex);
}
void CVertexArray::setAttribute(VertexAttribute_t const &p_attrib) const {
	glVertexArrayAttribFormat(
		vertex_array_object_,
		p_attrib.index,
		p_attrib.size,
		componentTypeToGL(p_attrib.type),
		p_attrib.normalized,
		p_attrib.offset
	);
	glVertexArrayAttribBinding(vertex_array_object_, p_attrib.index, p_attrib.binding);
	glEnableVertexArrayAttrib(vertex_array_object_, p_attrib.binding);
}

void open_gl_debug_proc(GLenum source, GLenum type, GLuint const id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam) {
	std::string source_str = gl::toPrettyString(static_cast<gl::DebugSource>(source));
	std::string type_str = gl::toPrettyString(static_cast<gl::DebugType>(type));

	std::cout << "[" << source_str << "] " << type_str << " #" << id << ": " << message << '\n';
}