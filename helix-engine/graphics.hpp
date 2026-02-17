#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>

#include "types.hpp"
#include "graphics-enums.hpp"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"

extern void initGraphics();
extern void terminateGraphics();

#define gpuDebug(str) (printf("[%s:%d] %s\n", &std::string(__FILE__)[42], __LINE__, str))
#define gpuDebugf(str, ...) (printf("[%s:%d] ", &std::string(__FILE__)[42], __LINE__), printf(str, __VA_ARGS__), printf("\n"))

struct video_mode {
	int red_bits;
	int green_bits;
	int blue_bits;
	int refresh_rate;
};

struct window_config {
	bool transparent;
	bool resizable;
	bool fullscreen;
	bool decorated;
	std::optional<video_mode> videoMode;
};

class CWindow {
public:
	GLFWwindow *window;
	CWindow();
	CWindow(
		glm::ivec2 const &p_startingSize,
		std::optional<std::string> const &p_windowTitle = std::nullopt,
		std::optional<std::reference_wrapper<CWindow>> const &p_sharedWindow = std::nullopt,
		std::optional<window_config> const &p_config = std::nullopt
	);
	~CWindow();

	// no copy no move
	CWindow(CWindow const& window) = delete;
	CWindow(CWindow&& window) = delete;
	CWindow& operator=(CWindow const& window) = delete;
	CWindow& operator=(CWindow&& window) = delete;

	[[nodiscard]] glm::ivec2 getSize() const;
	void setSize(glm::ivec2 const& size) const;

	void makeContextCurrent() const;
	void swapBuffers() const;
};

// CProgram

class CShader;

class CProgram {
	inline static u32 program_in_use_ = 0xFFFFFFFFu;
	u32 program_object_;
public:
	CProgram();
	~CProgram();

	CProgram(CProgram const& program) = delete;
	CProgram(CProgram&& program) = delete;
	CProgram& operator=(CProgram const& program) = delete;
	CProgram& operator=(CProgram&& program) = delete;

	void attach(CShader const &p_shaderObject) const;

	void link() const;
	void use() const;
	
	[[nodiscard]] bool inUse() const;
	[[nodiscard]] i32 uniformLocation(std::string const &p_name) const;

	void setUniform(i32 uniform, glm::mat4 const &p_matrix, bool transposed = false) const;
	void setUniform(i32 uniform, glm::mat3 const &p_matrix, bool transposed = false) const;
	void setUniform(i32 uniform, glm::mat2 const &p_matrix, bool transposed = false) const;

	void setUniform(i32 uniform, std::vector<glm::mat4> const &p_matrices, bool transposed = false) const;
	void setUniform(i32 uniform, std::vector<glm::mat3> const &p_matrices, bool transposed = false) const;
	void setUniform(i32 uniform, std::vector<glm::mat2> const &p_matrices, bool transposed = false) const;

	void setUniform(i32 uniform, glm::vec4 const &p_vector) const;
	void setUniform(i32 uniform, glm::vec3 const &p_vector) const;
	void setUniform(i32 uniform, glm::vec2 const &p_vector) const;

	void setUniform(i32 uniform, std::vector<glm::vec4> const &p_vectors) const;
	void setUniform(i32 uniform, std::vector<glm::vec3> const &p_vectors) const;
	void setUniform(i32 uniform, std::vector<glm::vec2> const &p_vectors) const;

	void setUniform(i32 uniform, glm::ivec4 const &p_vector) const;
	void setUniform(i32 uniform, glm::ivec3 const &p_vector) const;
	void setUniform(i32 uniform, glm::ivec2 const &p_vector) const;

	void setUniform(i32 uniform, std::vector<glm::ivec4> const &p_vectors) const;
	void setUniform(i32 uniform, std::vector<glm::ivec3> const &p_vectors) const;
	void setUniform(i32 uniform, std::vector<glm::ivec2> const &p_vectors) const;

	void setUniform(i32 uniform, glm::bvec4 const &p_vector) const;
	void setUniform(i32 uniform, glm::bvec3 const &p_vector) const;
	void setUniform(i32 uniform, glm::bvec2 const &p_vector) const;

	/*
	void setUniform(i32 const uniform, std::vector<glm::bvec4> const &p_vectors) const;
	void setUniform(i32 const uniform, std::vector<glm::bvec3> const &p_vectors) const;
	void setUniform(i32 const uniform, std::vector<glm::bvec2> const &p_vectors) const;
	*/
	
	void setUniform(i32 uniform, i32 value) const;
	void setUniform(i32 uniform, u32 value) const;
	void setUniform(i32 uniform, i64 value) const;
	void setUniform(i32 uniform, u64 value) const;
};
// CShader

class CShader {
	u32 shader_object_;
	gl::ShaderType shader_type_;

public:
	CShader(gl::ShaderType p_shaderType = gl::ShaderType::VertexShader);
	CShader( std::string const &p_source, gl::ShaderType p_shaderType = gl::ShaderType::VertexShader);
	~CShader();

	// nocopy
	CShader(CShader const& p_shader) = delete;
	CShader(CShader&& p_shader) = delete;
	CShader& operator=(CShader const& p_shader) = delete;
	CShader& operator=(CShader&& p_shader) = delete;

	void compile() const;
	void setSource(std::string const &p_source) const;
	[[nodiscard]] std::string source() const;
	[[nodiscard]] std::string infoLog() const;
	[[nodiscard]] gl::ShaderType type() const;

	[[nodiscard]] i32 compileStatus() const;

	friend class CProgram;
};

// CTexture

class CTexture {
	u32 texture_object_;

public:
	CTexture(gl::TextureTarget p_textureTarget);
	~CTexture();

	CTexture(CTexture const& p_texture) = delete;
	CTexture(CTexture&& p_texture) = delete;
	CTexture& operator=(CTexture const& p_texture) = delete;
	CTexture& operator=(CTexture&& p_texture) = delete;

	[[nodiscard]] i32 intParam(gl::GetTextureParameter p_param) const;
	void setIntParam(gl::GetTextureParameter p_param, i32 p_intParameter) const;

	[[nodiscard]] u32 uintParam(gl::GetTextureParameter p_param) const;
	void setUIntParam(gl::GetTextureParameter p_param, u32 p_uintParameter) const;
	
	[[nodiscard]] std::vector<i32> intVecParam(gl::GetTextureParameter p_param) const;
	void setIntVecParam(gl::GetTextureParameter p_param, std::vector<i32> const& p_vecParameter) const;

	[[nodiscard]] f32 getFloatParam(gl::GetTextureParameter p_param) const;
	void setFloatParam(gl::GetTextureParameter p_param, f32 p_floatParameter) const;

	void generateMipmap() const;

	void bindTextureUnit(u32 unit) const;

	void allocate(glm::ivec2 const &size, i32 levels, gl::InternalFormat internalFormat) const;
	void setImage2D(void const *data, i32 level, glm::ivec2 const &offset, glm::ivec2 const &size, gl::PixelFormat format = gl::PixelFormat::Rgba, gl::PixelType type = gl::PixelType::Byte) const;
};

// CBuffer

class CVertexArray;

class CBuffer {
	inline static u32 bound_object_ = 0xFFFFFFFFu;
	u32 buffer_object_;
	bool is_deleted_;

public:
	CBuffer() : is_deleted_(false) {
		glCreateBuffers(1, &buffer_object_);
		gpuDebugf("Buffer #%u has been born.", buffer_object_);
	}

	CBuffer(u32 const uiBufferObject) : buffer_object_(uiBufferObject), is_deleted_(false) {
	}
	
	~CBuffer() {
		if (!is_deleted_) {
			gpuDebugf("Buffer #%u is being deleted.", buffer_object_);
			glDeleteBuffers(1, &buffer_object_);
		}
	}

	template <std::size_t N>
	static void deleteBuffers(CBuffer (&buffers)[N]) {
		std::array<GLuint, N> objects;
		for (size_t i = 0; i < N; i++) {
			if (!buffers[i].is_deleted_) {
				objects[i] = buffers[i].buffer_object_;
				buffers[i].is_deleted_ = true;
			}
		}
		glDeleteBuffers(N, objects.data());
	}

	template <std::size_t N>
	static void deleteBuffers(std::shared_ptr<CBuffer> (&buffers)[N]) {
		std::array<GLuint, N> objects;
		for (size_t i = 0; i < N; i++) {
			if (!(buffers[i]->is_deleted_)) {
				objects[i] = buffers[i]->buffer_object_;
				buffers[i]->is_deleted_ = true;
			}
		}
		glDeleteBuffers(N, objects.data());
	}

	template <std::size_t N>
	static std::array<std::shared_ptr<CBuffer>, N> createBuffers() {
		std::array<std::reference_wrapper<CBuffer>, N> buffers{};
		std::array<u32, N> objects{};
		glCreateBuffers(N, objects.data());
		for (size_t i = 0; i < N; i++)
			buffers[i] = std::make_shared<CBuffer>({objects[i]});
		return buffers;
	}

	CBuffer(CBuffer const &) = delete;
	CBuffer(CBuffer &&) = delete;
	CBuffer& operator=(CBuffer const& p) = delete;
	CBuffer& operator=(CBuffer&& p) = delete;

	void bind() const;
	void unbind() const;

	void allocStorage(std::size_t const p_szSize, void const *p_pData, gl::BufferStorageMask p_eFlags) const {
		glNamedBufferStorage(buffer_object_, static_cast<GLsizeiptr>(p_szSize), p_pData, static_cast<GLbitfield>(p_eFlags));
	}

	void setData(std::size_t const p_szSize, void const *p_pData, gl::BufferUsageARB p_eUsage) const {
		glNamedBufferData(buffer_object_, static_cast<GLsizeiptr>(p_szSize), p_pData, static_cast<GLenum>(p_eUsage));
	}

	void setSubData(std::size_t const p_szSize, i64 const p_sziOffset, void const *p_pData) const {
		glNamedBufferSubData(buffer_object_, static_cast<GLintptr>(p_sziOffset), static_cast<GLsizeiptr>(p_szSize), p_pData);
	}

	void invalidateData() const {
		glInvalidateBufferData(buffer_object_);
	}

	void invalidateSubData(i64 const p_off, i64 const p_len) const {
		glInvalidateBufferSubData(buffer_object_, p_len, p_off);
	}

	void bindBufferBase(gl::BufferTargetARB const p_target, u32 const p_index) const {
		glBindBufferBase(static_cast<GLenum>(p_target), p_index, buffer_object_);
	}

	void bindBufferBase(gl::BufferTargetARB const p_target, u32 const p_index, i64 const p_offset, i64 const p_size) const {
		glBindBufferRange(static_cast<GLenum>(p_target), p_index, buffer_object_, p_offset, p_size);
	}

	friend class CVertexArray;
};

enum class EComponentType : std::uint8_t {
	HALF_FLOAT,
	SINGLE_FLOAT,
	DOUBLE_FLOAT,
	FIXED_FLOAT,
	SIGNED_BYTE,
	UNSIGNED_BYTE,
	SIGNED_SHORT,
	UNSIGNED_SHORT,
	SIGNED_INT,
	UNSIGNED_INT,
	INT_2_10_10_10,
	UNSIGNED_INT_2_10_10_10,
	UNSIGNED_INT_10F_11F_11F,
};

constexpr static GLenum componentTypeToGL(EComponentType const type) {
	switch (type) {
		case EComponentType::HALF_FLOAT: return GL_HALF_FLOAT;
		case EComponentType::SINGLE_FLOAT: return GL_FLOAT;
		case EComponentType::DOUBLE_FLOAT: return GL_DOUBLE;
		case EComponentType::FIXED_FLOAT: return GL_FIXED;
		case EComponentType::SIGNED_BYTE: return GL_BYTE;
		case EComponentType::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
		case EComponentType::SIGNED_SHORT: return GL_SHORT;
		case EComponentType::UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
		case EComponentType::SIGNED_INT: return GL_INT;
		case EComponentType::UNSIGNED_INT: return GL_UNSIGNED_INT;
		case EComponentType::INT_2_10_10_10: return GL_INT_2_10_10_10_REV;
		case EComponentType::UNSIGNED_INT_2_10_10_10: return GL_UNSIGNED_INT_2_10_10_10_REV;
		case EComponentType::UNSIGNED_INT_10F_11F_11F: return GL_UNSIGNED_INT_10F_11F_11F_REV;
	}
	return GL_NONE;
}

struct VertexAttribute_t {
	i32 index = 0; //< The index of the attribute, defines where the location is in GLSL. I.e. layout (location = 5) in vec3 aFoo; means that this value would be 5.
	i32 binding = 0; //< The binding to a particular vertex buffer.
	i32 size = 1; //< number of elements, used for vectors and such.
	i32 stride = 0; //< (opt) the length of each vertex data.
	u32 offset = 0u; //< (opt) (requires stride) the offset in each buffer stride for each value.
	EComponentType type = EComponentType::UNSIGNED_BYTE;
	bool normalized = false;
};

class CVertexArray {
public:
	gl::PrimitiveType primitive_type = gl::PrimitiveType::Triangles;
	gl::DrawElementsType draw_elements_type = gl::DrawElementsType::UnsignedByte;
	i32 elements_count = 0;
	
private:
	inline static u32 bound_object_ = 0xFFFFFFFFu;
	u32 vertex_array_object_;
	bool is_deleted_;
public:
	CVertexArray() : vertex_array_object_(0), is_deleted_(false) {
		glCreateVertexArrays(1, &vertex_array_object_);
		gpuDebugf("Vertex Array #%u has been born", vertex_array_object_);
	}

	~CVertexArray() {
		if (!is_deleted_) {
			gpuDebugf("Vertex Array #%u destroyed", vertex_array_object_);
			glDeleteVertexArrays(1, &vertex_array_object_);
		}
	}
	
	CVertexArray(CVertexArray const&) = delete;
	//CVertexArray(CVertexArray&&) = delete;
	CVertexArray& operator=(CVertexArray const& p) = delete;
	//CVertexArray& operator=(CVertexArray&& p) = delete;

	void bind() const {
		if (bound_object_ == vertex_array_object_)
			return;
		glBindVertexArray(vertex_array_object_);
		bound_object_ = vertex_array_object_;
	}
	
	void unbind() const {
		if (bound_object_ == vertex_array_object_) {
			glBindVertexArray(0);
			bound_object_ = 0;
		}
	}

	void enableAttribute(u32 const p_bindingindex) const;

	void setAttribute(VertexAttribute_t const &p_attrib) const;

	void setVertexBuffer(u32 const p_bindingindex, CBuffer const &buffer, i32 const p_stride, i64 const p_offset = 0) const {
		glVertexArrayVertexBuffer(vertex_array_object_, p_bindingindex, buffer.buffer_object_, p_offset, p_stride);
	}

	void setElementBuffer(CBuffer const &buffer) const {
		glVertexArrayElementBuffer(vertex_array_object_, buffer.buffer_object_);
	}

	[[nodiscard]] bool bound() const {
		return bound_object_ == vertex_array_object_;
	}

	void drawArrays(gl::PrimitiveType prim = gl::PrimitiveType::Triangles, i32 const first = 0, i32 const count = 0) const {
		bind();
		glDrawArrays(static_cast<GLenum>(prim), first, count);
	}

	void drawArraysInstanced(gl::PrimitiveType prim, i32 const first, i32 const count, i32 const instances) const {
		bind();
		glDrawArraysInstanced(static_cast<GLenum>(prim), first, count, instances);
	}

	void drawElements(gl::PrimitiveType prim = gl::PrimitiveType::Triangles, gl::DrawElementsType elem = gl::DrawElementsType::UnsignedByte, i32 const count = 0) const {
		bind();
		glDrawElements(static_cast<GLenum>(prim), count, static_cast<GLenum>(elem), nullptr);
	}

	void draw() const {
		bind();
		glDrawElements(static_cast<GLenum>(primitive_type), elements_count, static_cast<GLenum>(draw_elements_type), nullptr);
	}
};

extern void APIENTRY open_gl_debug_proc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam);