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

#include "os.hpp"
#include "util.hpp"
#include "engine/filesystem.hpp"
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

bool gpu::check(char const *where, _STD size_t const line) {
	/*
	try {
		gl::enum_t err = glGetError();
		while (err != 0) {
			printf("[%s:%llu] OpenGL has encountered an error: \"%s\"", where, line, gl::to_pretty_string(static_cast<gl::ErrorCode>(err)));
		
			throw _STD exception("OpenGL error");
			err = glGetError();
		}
		return true;
	}
	catch (std::exception const &e) {
		std::cout << "Failed to log GL error! " << e.what() << '\n';
	}
	*/
	return true;
}

// CWindow Impl

CWindow::CWindow(
	glm::ivec2 const &p_startingSize,
	_STD optional<_STD string> const &p_windowTitle,
	_STD optional<_STD reference_wrapper<CWindow>> const &p_sharedWindow,
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

	glfwWindowHint(GLFW_SAMPLES, 8);
	
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

bool CWindow::shouldClose() const {
	return glfwWindowShouldClose(window);
}

void CWindow::hide() const {
	glfwHideWindow(window);
}

void CWindow::show() const {
	glfwShowWindow(window);
}

void CWindow::setVisible(bool const visible) const {
	if (visible) show(); else hide();
}

bool CWindow::visible() const {
	return glfwGetWindowAttrib(window, GLFW_VISIBLE) == GLFW_TRUE;
}

void CWindow::setFramebufferSizeCallback(GLFWframebuffersizefun const fun) const {
	glfwSetFramebufferSizeCallback(window, fun);
}

void CWindow::makeContextCurrent() const {
	glfwMakeContextCurrent(window);
}
void CWindow::swapBuffers() const {
	glfwSwapBuffers(window);
}

// CProgram

CProgram::CProgram() : program_object_(glCreateProgram()) {
}

CProgram::~CProgram() {
	glDeleteProgram(program_object_);
}

void CProgram::attach(CShader &p_shaderObject) {
	glAttachShader(program_object_, p_shaderObject.shader_object_); gpu_check;
	shaders_.push_back(_STD ref(p_shaderObject));
}

void CProgram::setLabel(_STD string_view const p_label) const {
	glObjectLabel(GL_PROGRAM, program_object_, static_cast<gl::sizei_t>(p_label.size()), p_label.data()); gpu_check;
}

void CProgram::link() const {
	glLinkProgram(program_object_); gpu_check;
}

void CProgram::use() const {
	if (program_in_use_ == program_object_)
		return;
	glUseProgram(program_object_); gpu_check;
	program_in_use_ = program_object_;
}

void CProgram::integrityCheck() {
	bool any_failed_checks = false;
	for (auto &shader : shaders_) {
		bool const check = shader.get().integrityCheck();
		any_failed_checks |= check;
	}
	if (any_failed_checks) {
		_STD cout << "Shader has been updated! Recompiling!\n";
		bool const currently_in_use = program_in_use_ == program_object_;
		for (auto &shader : shaders_) {
			glAttachShader(program_object_, shader.get().shader_object_);
			gpu_check;
		}
		glLinkProgram(program_object_); gpu_check;
	}
}

bool CProgram::inUse() const {
	return program_in_use_ == program_object_;
}

i32 CProgram::uniformLocation(_STD string const &p_name) const {
	return glGetUniformLocation(program_object_, p_name.c_str());
}

void CProgram::setUniform(i32 const uniform, glm::mat4 const &p_matrix, bool const transposed) const { glProgramUniformMatrix4fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }
void CProgram::setUniform(i32 const uniform, glm::mat3 const &p_matrix, bool const transposed) const { glProgramUniformMatrix3fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }
void CProgram::setUniform(i32 const uniform, glm::mat2 const &p_matrix, bool const transposed) const { glProgramUniformMatrix2fv(program_object_, uniform, 1, transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrix)); }

void CProgram::setUniform(i32 const uniform, _STD vector<glm::mat4> const &p_matrices, bool const transposed) const { glProgramUniformMatrix4fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }
void CProgram::setUniform(i32 const uniform, _STD vector<glm::mat3> const &p_matrices, bool const transposed) const { glProgramUniformMatrix3fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }
void CProgram::setUniform(i32 const uniform, _STD vector<glm::mat2> const &p_matrices, bool const transposed) const { glProgramUniformMatrix2fv(program_object_, uniform, static_cast<GLsizei>(p_matrices.size()), transposed ? GL_TRUE : GL_FALSE, glm::value_ptr(p_matrices[0])); }

void CProgram::setUniform(i32 const uniform, glm::vec4 const &p_vector) const { glProgramUniform4f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void CProgram::setUniform(i32 const uniform, glm::vec3 const &p_vector) const { glProgramUniform3f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void CProgram::setUniform(i32 const uniform, glm::vec2 const &p_vector) const { glProgramUniform2f(program_object_, uniform, p_vector.x, p_vector.y); }

void CProgram::setUniform(i32 const uniform, _STD vector<glm::vec4> const &p_vectors) const { glProgramUniform4fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void CProgram::setUniform(i32 const uniform, _STD vector<glm::vec3> const &p_vectors) const { glProgramUniform3fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void CProgram::setUniform(i32 const uniform, _STD vector<glm::vec2> const &p_vectors) const { glProgramUniform2fv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }

void CProgram::setUniform(i32 const uniform, glm::ivec4 const &p_vector) const { glProgramUniform4i(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void CProgram::setUniform(i32 const uniform, glm::ivec3 const &p_vector) const { glProgramUniform3i(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void CProgram::setUniform(i32 const uniform, glm::ivec2 const &p_vector) const { glProgramUniform2i(program_object_, uniform, p_vector.x, p_vector.y); }

void CProgram::setUniform(i32 const uniform, _STD vector<glm::ivec4> const &p_vectors) const { glProgramUniform4iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void CProgram::setUniform(i32 const uniform, _STD vector<glm::ivec3> const &p_vectors) const { glProgramUniform3iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
void CProgram::setUniform(i32 const uniform, _STD vector<glm::ivec2> const &p_vectors) const { glProgramUniform2iv(program_object_, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }

void CProgram::setUniform(i32 const uniform, glm::bvec4 const &p_vector) const { glProgramUniform4f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z, p_vector.w); }
void CProgram::setUniform(i32 const uniform, glm::bvec3 const &p_vector) const { glProgramUniform3f(program_object_, uniform, p_vector.x, p_vector.y, p_vector.z); }
void CProgram::setUniform(i32 const uniform, glm::bvec2 const &p_vector) const { glProgramUniform2f(program_object_, uniform, p_vector.x, p_vector.y); }

//void CProgram::setUniform(i32 uniform, _STD vector<glm::bvec4> const &p_vectors) const { glProgramUniform4iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), static_cast<const GLint *>(glm::value_ptr(p_vectors[0]))); }
//void CProgram::setUniform(i32 uniform, _STD vector<glm::bvec3> const &p_vectors) const { glProgramUniform3iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }
//void CProgram::setUniform(i32 uniform, _STD vector<glm::bvec2> const &p_vectors) const { glProgramUniform2iv(programObject, uniform, static_cast<GLsizei>(p_vectors.size()), glm::value_ptr(p_vectors[0])); }


void CProgram::setUniform(i32 const uniform, i32 const value) const { glProgramUniform1i(program_object_, uniform, value); }
void CProgram::setUniform(i32 const uniform, u32 const value) const { glProgramUniform1ui(program_object_, uniform, value); }
void CProgram::setUniform(i32 const uniform, i64 const value) const { glProgramUniform1i64ARB(program_object_, uniform, value); }
void CProgram::setUniform(i32 const uniform, u64 const value) const { glProgramUniform1ui64ARB(program_object_, uniform, value); }

// CShader

CShader::CShader(gl::ShaderType p_shaderType) :
	shader_object_(glCreateShader(static_cast<GLenum>(p_shaderType))),
	shader_type_(p_shaderType) {
}

CShader::CShader(gl::ShaderType p_shaderType, std::string_view p_fileName) :
	shader_object_(glCreateShader(static_cast<GLenum>(p_shaderType))),
	shader_type_(p_shaderType) {
	setFileSource(p_fileName);
	compile();
	assertStatus();
}

CShader::CShader(_STD string const &p_source, gl::ShaderType p_shaderType) :
	shader_object_(glCreateShader(static_cast<GLenum>(p_shaderType))),
	shader_type_(p_shaderType) {
	setSource(p_source);
}

CShader::~CShader() {
	glDeleteShader(shader_object_);
}

void CShader::setLabel(_STD string_view const p_label) const {
	glObjectLabel(GL_SHADER, shader_object_,
		static_cast<GLsizei>(p_label.size()),
		p_label.data()); gpu_check;
}

void CShader::compile() const {
	glCompileShader(shader_object_);gpu_check;
}

void CShader::setSource(std::string_view p_source, std::string_view p_file_name) {
	char const *sources = p_source.data();
	GLsizei const length = static_cast<GLsizei>(p_source.size());
	glShaderSource(shader_object_, 1, &sources, &length); gpu_check;
	if (p_file_name.empty()) return;
	source_file_ = _STD string(p_file_name.data(), p_file_name.size());
	_STD function const lambda = [this](EFileAction const action) {
		if (action == EFileAction::Modified)
			recompile();
	};
	CFileSystemMonitor::instance->createListener(p_file_name, lambda);
}

void CShader::setFileSource(std::string_view const p_file_name) {
	_STD string file_name(p_file_name.data(), p_file_name.size());
	_STD ifstream source_stream(file_name);
	source_stream.seekg(0, _STD ios::end);
	_STD size_t source_size = source_stream.tellg();
	_STD string source_content(source_size + 1, '\0');
	source_stream.seekg(0, _STD ios::beg);
	source_stream.read(source_content.data(), static_cast<_STD streamsize>(source_size));
	setSource(source_content, p_file_name);
}
void CShader::assertStatus() const {
	if (!compileStatus()) _UNLIKELY {
		_STD string const info_log = infoLog();
		_STD cout << info_log << "\n\n\n";
		throw std::runtime_error(info_log);
	}
}

void CShader::recompile() {
	
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
}

_STD string CShader::source() const {
	gl::sizei_t size;
	glGetShaderSource(shader_object_, 0, &size, nullptr); gpu_check;
	auto const log = new gl::char_t[size];
	_STD memset(log, 0, size);
	glGetShaderSource(shader_object_, size, &size, log); gpu_check;
	_STD string logStr(log);
	delete[] log;
	return logStr;
}

_STD string CShader::infoLog() const {
	gl::int32_t size;
	glGetShaderiv(shader_object_, static_cast<GLenum>(gl::ShaderParameterName::InfoLogLength), &size);
	_STD cout << size << '\n';
	auto const log = new gl::char_t[size];
	_STD memset(log, '\0', size);
	glGetShaderInfoLog(shader_object_, size, nullptr, log);
	gpu_check;
	_STD cout << log << '\n';
	_STD string logStr(log);
	delete[] log;
	return logStr;
}

gl::ShaderType CShader::type() const {
	return shader_type_;
}

i32 CShader::compileStatus() const {
	i32 value;
	glGetShaderiv(shader_object_, GL_COMPILE_STATUS, &value);
	_STD cout << "compile status: " << value << '\n';
	return value;
}

bool CShader::integrityCheck() {
	if (needs_relinking_) {
		needs_relinking_ = false;
		return true;
	}
	return false;
}

// CTexture

CTexture::CTexture(gl::TextureTarget p_textureTarget) {
	glCreateTextures(static_cast<GLenum>(p_textureTarget), 1, &texture_object_);
}

CTexture::CTexture(u32 const existing_texture_object_) : texture_object_(existing_texture_object_) {
}

CTexture::~CTexture() {
	glDeleteTextures(1, &texture_object_);
}

void CTexture::setLabel(_STD string_view const name) const {
	glObjectLabel(GL_TEXTURE, texture_object_, static_cast<GLsizei>(name.size()), name.data());
}

i32 CTexture::intParam(gl::GetTextureParameter parameter) const {
	i32 iValue;
	glGetTextureParameteriv(texture_object_, static_cast<GLenum>(parameter), &iValue);
	return iValue;
}

void CTexture::setIntParam(gl::GetTextureParameter parameter, i32 const value) const {
	glTextureParameteri(texture_object_, static_cast<GLenum>(parameter), value); gpu_check;
}

u32 CTexture::uintParam(gl::GetTextureParameter parameter) const {
	u32 uiValue;
	glGetTextureParameterIuiv(texture_object_, static_cast<GLenum>(parameter), &uiValue);
	return uiValue;
}

void CTexture::setUIntParam(gl::GetTextureParameter parameter, u32 const value) const {
	glTextureParameterIuiv(texture_object_, static_cast<GLenum>(parameter), &value); gpu_check;
}

_STD vector<i32> CTexture::intVecParam(gl::GetTextureParameter parameter) const {
	return {};
}

void CTexture::setIntVecParam(gl::GetTextureParameter parameter, _STD vector<i32> const &value) const {
}

void CTexture::generateMipmap() const {
	glGenerateTextureMipmap(texture_object_); gpu_check;
}

void CTexture::setAnisotropicFilteringEnabled(bool enabled) {
	glTextureParameterf(texture_object_, GL_TEXTURE_MAX_ANISOTROPY, enabled ? 0.0f : 1.0f); gpu_check;
	anisotropic_filtering_enabled_ = enabled;
}

void CTexture::enableAnisotropicFiltering() {
	setAnisotropicFilteringEnabled(true);
}

void CTexture::disableAnisotropicFiltering() {
	setAnisotropicFilteringEnabled(false);
}

bool CTexture::isAnisotropicFilteringEnabled() const {
	return anisotropic_filtering_enabled_;
}
void CTexture::bindImage(gl::uint32_t unit, gl::InternalFormat format, gl::BufferAccessARB access, gl::int32_t level, bool layered, gl::int32_t layer) const {
	glBindImageTexture(
		unit,
		texture_object_,
		level,
		layered,
		layer,
		static_cast<GLenum>(access),
		static_cast<GLenum>(format)
	); gpu_check;
}

void CTexture::bindImage(image_descriptor const &descriptor) const {
	glBindImageTexture(
		descriptor.unit,
		texture_object_,
		descriptor.level,
		descriptor.layered,
		descriptor.layer,
		static_cast<GLenum>(descriptor.access),
		static_cast<GLenum>(descriptor.format)
	); gpu_check;
}

void CTexture::bindTextureUnit(u32 const unit) const {
	glBindTextureUnit(unit, texture_object_); gpu_check;
}

void CTexture::allocate(glm::ivec2 const &size, i32 const levels, gl::InternalFormat format) {
	internal_format_=format;
	glTextureStorage2D(texture_object_, levels, static_cast<GLenum>(format), size.x, size.y);
	gpu_check;
}

void CTexture::uploadImage2D(void const *data, i32 const level, glm::ivec2 const &offset, glm::ivec2 const &size, gl::PixelFormat format, gl::PixelType type) {
	assert(data != nullptr);
	glTextureSubImage2D(
		texture_object_, level,
		offset.x, offset.y,
		size.x, size.y,
		static_cast<GLenum>(format),
		static_cast<GLenum>(type),
		data
	);
	this->pixel_format_ = format;
	this->pixel_type_ = type;
	gpu_check;
}

void CTexture::setCompressedImage2D(void const *data, i32 const level, glm::ivec2 const &offset, glm::ivec2 const &size, gl::PixelFormat format, gl::sizei_t const pixel_size) {
	assert(data != nullptr);
	glCompressedTextureSubImage2D(
		texture_object_,
		level,
		offset.x, offset.y,
		size.x, size.y,
		static_cast<GLenum>(format),
		pixel_size,
		data
	);
	pixel_format_ = format;
	
	gpu_check;
}

glm::ivec2 CTexture::levelSize2D(i32 const level, glm::ivec2 const &size) const {
	i32 w, h;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_WIDTH, &w); gpu_check;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_HEIGHT, &h);
	return glm::ivec2(w, h);
}

gl::int32_t CTexture::compressedImageSize(i32 const level) const {
	i32 size;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size); gpu_check;
	return size;
}

std::vector<u8> CTexture::compressedImageData(i32 const level) const {
	i32 size = compressedImageSize(level);
	std::vector<u8> pixels(size);
	glGetCompressedTextureImage(texture_object_, level, size, pixels.data()); gpu_check;
	return pixels;
}

void CTexture::compressedImageData(std::vector<u8> &pixels, i32 const level) const {
	assert(compressed(level));
	i32 const size = compressedImageSize(level);
	pixels.resize(size);
	glGetCompressedTextureImage(texture_object_, level, size, pixels.data()); gpu_check;
}

gl::int32_t CTexture::imageDataSize(i32 level) const {
	i32 width, height;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_WIDTH, &width); gpu_check;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_HEIGHT, &height);
	switch (this->pixel_format_) {
		case gl::PixelFormat::Rgb:
			return width * height * 3;
		case gl::PixelFormat::Rgba:
			return width * height * 4;
		default:
			return width * height * 2;
	}
	return 0;
}

void CTexture::imageData(std::vector<u8> &pixels, i32 const level) const {
	i32 const size = imageDataSize(level);
	pixels.resize(size);
	glGetTextureImage(
		texture_object_,
		level,
		static_cast<GLenum>(pixel_format_),
		static_cast<GLenum>(pixel_type_),
		size,
		pixels.data()
	);
	gpu_check;
}

bool CTexture::compressed(i32 const level) const {
	i32 is_compressed;
	glGetTextureLevelParameteriv(texture_object_, level, GL_TEXTURE_COMPRESSED, &is_compressed);
	return is_compressed == GL_TRUE;
}

bool CTexture::isValid() const {
	return glIsTexture(texture_object_) == GL_TRUE;
}

void CBuffer::setLabel(_STD string const &p_label) const {
	glObjectLabel(static_cast<GLenum>(gl::ObjectIdentifier::Buffer), buffer_object_, static_cast<GLsizei>(p_label.length()), p_label.c_str());
	gpu_check;
}

_STD size_t CBuffer::size() const {
	i32 i_size = 0;
	glGetNamedBufferParameteriv(buffer_object_, GL_BUFFER_SIZE, &i_size);
	return i_size;
}

bool CBuffer::immutable() const {
	i32 i_immutable = 0;
	glGetNamedBufferParameteriv(buffer_object_, GL_BUFFER_IMMUTABLE_STORAGE, &i_immutable);
	return i_immutable == GL_TRUE;
}



void CVertexArray::setLabel(_STD string_view const p_label) const {
	glObjectLabel(GL_VERTEX_ARRAY, vertex_array_object_, static_cast<GLsizei>(p_label.size()), p_label.data());
	gpu_check;
}

void CVertexArray::enableAttribute(u32 const p_bindingindex) const {
	glEnableVertexArrayAttrib(vertex_array_object_, p_bindingindex);
	gpu_check;
}
void CVertexArray::setAttribute(VertexAttribute_t const &p_attrib) const {
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


CRenderbuffer::CRenderbuffer() : renderbuffer_object_(0u) {
	glCreateRenderbuffers(1, &renderbuffer_object_);
}

CRenderbuffer::~CRenderbuffer() {
	glDeleteRenderbuffers(1, &renderbuffer_object_);
}

void CRenderbuffer::allocateStorage(glm::ivec2 const &size, gl::InternalFormat internalFormat) const {
	glNamedRenderbufferStorage(
		renderbuffer_object_,
		static_cast<GLenum>(internalFormat),
		size.x, size.y
	); gpu_check;
}

void CRenderbuffer::allocateStorageMultisample(glm::ivec2 const &size, i32 const samples, gl::InternalFormat internalFormat) const {
	glNamedRenderbufferStorageMultisample(
		renderbuffer_object_,
		samples,
		static_cast<GLenum>(internalFormat),
		size.x, size.y
	); gpu_check;
}

u32 CFramebuffer::bound_framebuffer_ = 0xFFFFFFFFu;
u32 CFramebuffer::bound_draw_framebuffer_ = 0xFFFFFFFFu;
u32 CFramebuffer::bound_read_framebuffer_ = 0xFFFFFFFFu;

CFramebuffer::CFramebuffer(u32 const index) : framebuffer_object_(index) {}

CFramebuffer::CFramebuffer() {
	glCreateFramebuffers(1, &framebuffer_object_);
}

CFramebuffer::~CFramebuffer() {
	if (framebuffer_object_ != 0 && framebuffer_object_ != 0xFFFFFFFFu) { //< Don't delete the default framebuffer
		glDeleteFramebuffers(1, &framebuffer_object_);
	}
}

void CFramebuffer::bind(gl::FramebufferTarget target) const {
	switch (target) {
		case gl::FramebufferTarget::DrawFramebuffer:
			if (bound_draw_framebuffer_ == framebuffer_object_)
				return;
			bound_draw_framebuffer_ = framebuffer_object_;
			break;
		case gl::FramebufferTarget::ReadFramebuffer:
			if (bound_read_framebuffer_ == framebuffer_object_)
				return;
			bound_read_framebuffer_ = framebuffer_object_;
			break;
		case gl::FramebufferTarget::Framebuffer:
			if (bound_framebuffer_ == framebuffer_object_)
				return;
			bound_framebuffer_ = framebuffer_object_;
			break;
	}
	glBindFramebuffer(static_cast<gl::enum_t>(target), framebuffer_object_);
}

void CFramebuffer::unbind(gl::FramebufferTarget target) const {
	switch (target) {
		case gl::FramebufferTarget::DrawFramebuffer:
			if (bound_draw_framebuffer_ == 0 || bound_draw_framebuffer_ != framebuffer_object_)
				return;
			bound_draw_framebuffer_ = 0;
			break;
		case gl::FramebufferTarget::ReadFramebuffer:
			if (bound_read_framebuffer_ == 0 || bound_read_framebuffer_ != framebuffer_object_)
				return;
			bound_read_framebuffer_ = 0;
			break;
		case gl::FramebufferTarget::Framebuffer:
			if (bound_framebuffer_ == 0 || bound_framebuffer_ != framebuffer_object_)
				return;
			bound_framebuffer_ = 0;
			break;
	}
	glBindFramebuffer(static_cast<gl::enum_t>(target), 0);
}
void CFramebuffer::setLabel(_STD string_view const p_label) const {
	glObjectLabel(GL_FRAMEBUFFER, framebuffer_object_, static_cast<GLsizei>(p_label.size()), p_label.data());
}

void CFramebuffer::attachTexture(gl::ColorBuffer color_buffer, CTexture const &texture, i32 const level) const {
	glNamedFramebufferTexture(
		framebuffer_object_,
		static_cast<GLenum>(color_buffer),
		texture.texture_object_,
		level
	);
}

void CFramebuffer::attachRenderbuffer(CRenderbuffer const &renderbuffer, gl::FramebufferAttachment attachment) const {
	glNamedFramebufferRenderbuffer(
		framebuffer_object_,
		static_cast<GLenum>(attachment),
		GL_RENDERBUFFER,
		renderbuffer.renderbuffer_object_
	);
}

void CFramebuffer::setDrawBuffers(_STD vector<gl::ColorBuffer> const &buffers) const {
	glNamedFramebufferDrawBuffers(
		framebuffer_object_,
		static_cast<GLsizei>(buffers.size()),
		reinterpret_cast<GLenum const *>(buffers.data())
	);
}

gl::FramebufferStatus CFramebuffer::status() const
{
	gl::enum_t status = glCheckNamedFramebufferStatus(framebuffer_object_, GL_FRAMEBUFFER);
	return static_cast<gl::FramebufferStatus>(status);
}

void CFramebuffer::blit(CFramebuffer const &dest, glm::ivec4 const &src, glm::ivec4 const &dst, gl::bitfield_t const mask, gl::BlitFramebufferFilter filter) const {
	glBlitNamedFramebuffer(
		framebuffer_object_,
		dest.framebuffer_object_,
		src.x, src.y, src.z, src.w,
		dst.x, dst.y, dst.z, dst.w,
		mask,
		static_cast<GLenum>(filter)
	);
}

void open_gl_debug_proc(GLenum source, GLenum type, GLuint const id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam) {
	_STD string source_str = gl::toPrettyString(static_cast<gl::DebugSource>(source));
	_STD string type_str = gl::toPrettyString(static_cast<gl::DebugType>(type));

	_STD cout << "[" << source_str << "] " << type_str << " #" << id << ": " << message << '\n';
}

CFramebuffer default_framebuffer(0);