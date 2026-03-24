#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <thread>

#include "types.hpp"
#include "opengl_enums.hpp"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"

extern void initGraphics();
extern void terminateGraphics();

#define GPU_DEBUG 0

#if GPU_DEBUG == 1
#define gpuDebug(str) (printf("[%s:%d] %s\n", &_STD string(__FILE__)[42], __LINE__, str))
#define gpuDebugf(str, ...) (printf("[%s:%d] ", &_STD string(__FILE__)[42], __LINE__), printf(str, __VA_ARGS__), printf("\n"))
#else
#define gpuDebug(...)
#define gpuDebugf(...)
#endif

namespace gpu {
	extern bool check(char const *where, _STD size_t const line);
}

#if 1
#define gpu_check (::gpu::check(__FILE__, __LINE__))
#else
#define gpu_check
#endif
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
	_STD optional<video_mode> videoMode;
};

class Window {
public:
	GLFWwindow *window;
	Window();
	Window(
		glm::ivec2 const &p_startingSize,
		_STD optional<_STD string> const &p_windowTitle = _STD nullopt,
		_STD optional<_STD reference_wrapper<Window>> const &p_sharedWindow = _STD nullopt,
		_STD optional<window_config> const &p_config = _STD nullopt
	);
	~Window();

	// no copy no move
	Window(Window const& window) = delete;
	Window(Window&& window) = delete;
	Window& operator=(Window const& window) = delete;
	Window& operator=(Window&& window) = delete;

	_NODISCARD glm::ivec2 getSize() const;
	void setSize(glm::ivec2 const& size) const;

	_NODISCARD bool shouldClose() const;

	void hide() const;
	void show() const;
	void setVisible(bool visible) const;
	_NODISCARD bool visible() const;

	void setFramebufferSizeCallback(GLFWframebuffersizefun fun) const;

	void makeContextCurrent() const;
	void swapBuffers() const;
};

// Program

class Shader;

class Program {
	inline static u32 program_in_use_ = 0xFFFFFFFFu;
	u32 program_object_;

	_STD vector<_STD reference_wrapper<Shader>> shaders_;
	
public:
	Program();
	~Program();

	Program(Program const& program) = delete;
	Program(Program&& program) = delete;
	Program& operator=(Program const& program) = delete;
	Program& operator=(Program&& program) = delete;

	void attach(Shader &p_shaderObject);
	void setLabel(_STD string_view p_label) const;

	void link() const;
	void use() const;
	void integrityCheck();
	
	_NODISCARD bool inUse() const;
	_NODISCARD i32 uniformLocation(_STD string const &p_name) const;

	void setUniform(i32 uniform, glm::mat4 const &p_matrix, bool transposed = false) const;
	void setUniform(i32 uniform, glm::mat3 const &p_matrix, bool transposed = false) const;
	void setUniform(i32 uniform, glm::mat2 const &p_matrix, bool transposed = false) const;

	void setUniform(i32 uniform, _STD vector<glm::mat4> const &p_matrices, bool transposed = false) const;
	void setUniform(i32 uniform, _STD vector<glm::mat3> const &p_matrices, bool transposed = false) const;
	void setUniform(i32 uniform, _STD vector<glm::mat2> const &p_matrices, bool transposed = false) const;

	void setUniform(i32 uniform, glm::vec4 const &p_vector) const;
	void setUniform(i32 uniform, glm::vec3 const &p_vector) const;
	void setUniform(i32 uniform, glm::vec2 const &p_vector) const;

	void setUniform(i32 uniform, _STD vector<glm::vec4> const &p_vectors) const;
	void setUniform(i32 uniform, _STD vector<glm::vec3> const &p_vectors) const;
	void setUniform(i32 uniform, _STD vector<glm::vec2> const &p_vectors) const;

	void setUniform(i32 uniform, glm::ivec4 const &p_vector) const;
	void setUniform(i32 uniform, glm::ivec3 const &p_vector) const;
	void setUniform(i32 uniform, glm::ivec2 const &p_vector) const;

	void setUniform(i32 uniform, _STD vector<glm::ivec4> const &p_vectors) const;
	void setUniform(i32 uniform, _STD vector<glm::ivec3> const &p_vectors) const;
	void setUniform(i32 uniform, _STD vector<glm::ivec2> const &p_vectors) const;

	void setUniform(i32 uniform, glm::bvec4 const &p_vector) const;
	void setUniform(i32 uniform, glm::bvec3 const &p_vector) const;
	void setUniform(i32 uniform, glm::bvec2 const &p_vector) const;

	/*
	void setUniform(i32 const uniform, _STD vector<glm::bvec4> const &p_vectors) const;
	void setUniform(i32 const uniform, _STD vector<glm::bvec3> const &p_vectors) const;
	void setUniform(i32 const uniform, _STD vector<glm::bvec2> const &p_vectors) const;
	*/
	
	void setUniform(i32 uniform, i32 value) const;
	void setUniform(i32 uniform, u32 value) const;
	void setUniform(i32 uniform, i64 value) const;
	void setUniform(i32 uniform, u64 value) const;
};
// Shader

class Shader {
	u32 shader_object_;
	gl::ShaderType shader_type_;
	_STD string source_file_;
	bool needs_relinking_ = false;

public:
	Shader(gl::ShaderType p_shaderType = gl::ShaderType::VertexShader);
	Shader(gl::ShaderType p_shaderType, _STD string_view p_fileName);
	Shader(_STD string const &p_source, gl::ShaderType p_shaderType = gl::ShaderType::VertexShader);
	~Shader();

	// nocopy
	Shader(Shader const& p_shader) = delete;
	Shader(Shader&& p_shader) = delete;
	Shader& operator=(Shader const& p_shader) = delete;
	Shader& operator=(Shader&& p_shader) = delete;

	void setLabel(_STD string_view p_label) const;
	void compile() const;
	void setSource(_STD string_view p_source, _STD string_view p_file_name = "");
	void setFileSource(_STD string_view p_file_name);
	void assertStatus() const;

	void recompile();
	
	_NODISCARD _STD string source() const;
	_NODISCARD _STD string infoLog() const;
	_NODISCARD gl::ShaderType type() const;

	_NODISCARD i32 compileStatus() const;
	_NODISCARD bool integrityCheck();

	friend class Program;
};

// Texture
class Framebuffer;

struct image_descriptor {
	gl::InternalFormat  format;
	gl::BufferAccessARB access;
	gl::uint32_t unit = 0;
	gl::int32_t level = 0;
	gl::int32_t layer = 0;
	bool layered = false;
};

class Texture {
	u32 texture_object_;
	gl::InternalFormat internal_format_;
	gl::PixelFormat pixel_format_;
	gl::PixelType pixel_type_;
	bool anisotropic_filtering_enabled_ = false;

public:
	Texture(gl::TextureTarget p_textureTarget);
	Texture(u32 existing_texture_object_);
	~Texture();

	Texture(Texture const& p_texture) = delete;
	Texture(Texture&& p_texture) = delete;
	Texture& operator=(Texture const& p_texture) = delete;
	Texture& operator=(Texture&& p_texture) = delete;

	void setLabel(_STD string_view name) const;

	_NODISCARD i32 intParam(gl::GetTextureParameter parameter) const;
	void setIntParam(gl::GetTextureParameter parameter, i32 value) const;

	_NODISCARD u32 uintParam(gl::GetTextureParameter parameter) const;
	void setUIntParam(gl::GetTextureParameter parameter, u32 value) const;
	
	_NODISCARD _STD vector<i32> intVecParam(gl::GetTextureParameter parameter) const;
	void setIntVecParam(gl::GetTextureParameter parameter, _STD vector<i32> const& value) const;

	void setWrapMode(gl::TextureWrapMode wrap_mode, std::optional<gl::TextureWrapMode> wrap_s = std::nullopt, std::optional<gl::TextureWrapMode> wrap_t = std::nullopt) const;
	

	_NODISCARD f32 getFloatParam(gl::GetTextureParameter parameter) const;
	void setFloatParam(gl::GetTextureParameter parameter, f32 value) const;

	void generateMipmap() const;
	void setAnisotropicFilteringEnabled(bool enabled);
	void enableAnisotropicFiltering();
	void disableAnisotropicFiltering();
	_NODISCARD bool isAnisotropicFilteringEnabled() const;

	void bindImage(gl::uint32_t unit, gl::InternalFormat format, gl::BufferAccessARB access, gl::int32_t level = 0, bool layered = false, gl::int32_t layer = 0) const;
	void bindImage(image_descriptor const& descriptor) const;
	void bindTextureUnit(u32 unit) const;

	void allocate(glm::ivec2 const &size, i32 levels, gl::InternalFormat format);
	void uploadImage2D(void const *data, i32 level, glm::ivec2 const &offset, glm::ivec2 const &size, gl::PixelFormat format = gl::PixelFormat::Rgba, gl::PixelType type = gl::PixelType::Byte);
	void setCompressedImage2D(void const *data, i32 level, glm::ivec2 const &offset, glm::ivec2 const &size, gl::PixelFormat format = gl::PixelFormat::Rgba, gl::sizei_t pixel_size = 0);

	_NODISCARD glm::ivec2 levelSize2D(i32 level, glm::ivec2 const &size) const;
	_NODISCARD gl::int32_t compressedImageSize(i32 level = 0) const;
	_NODISCARD _STD vector<u8> compressedImageData(i32 level = 0) const;
	void compressedImageData(_STD vector<u8> &pixels, i32 level = 0) const;

	_NODISCARD gl::int32_t imageDataSize(i32 level = 0) const;
	void imageData(_STD vector<u8> &pixels, i32 level = 0) const;

	_NODISCARD bool compressed(i32 level = 0) const;

	_NODISCARD bool isValid() const;

private:
	template <gl::GetTextureParameter P, typename T>
	void setParamI(T value) const {
		setIntParam(P, static_cast<i32>(value));
	}
public:
	friend class Framebuffer;
};

// Buffer

class VertexArray;

class Buffer {
	inline static u32 bound_object_ = 0xFFFFFFFFu;
	u32 buffer_object_;
	bool is_deleted_;

public:
	Buffer() : is_deleted_(false) {
		glCreateBuffers(1, &buffer_object_);
		gpuDebugf("Buffer #%u has been born.", buffer_object_);
	}

	Buffer(u32 const uiBufferObject) : buffer_object_(uiBufferObject), is_deleted_(false) {
	}
	
	~Buffer() {
		if (!is_deleted_) {
			gpuDebugf("Buffer #%u is being deleted.", buffer_object_);
			glDeleteBuffers(1, &buffer_object_);
		}
	}

	template <_STD size_t N>
	static void deleteBuffers(Buffer (&buffers)[N]) {
		_STD array<GLuint, N> objects;
		for (size_t i = 0; i < N; i++) {
			if (!buffers[i].is_deleted_) {
				objects[i] = buffers[i].buffer_object_;
				buffers[i].is_deleted_ = true;
			}
		}
		glDeleteBuffers(N, objects.data());
	}

	template <_STD size_t N>
	static void deleteBuffers(_STD shared_ptr<Buffer> (&buffers)[N]) {
		_STD array<GLuint, N> objects;
		for (size_t i = 0; i < N; i++) {
			if (!(buffers[i]->is_deleted_)) {
				objects[i] = buffers[i]->buffer_object_;
				buffers[i]->is_deleted_ = true;
			}
		}
		glDeleteBuffers(N, objects.data());
	}

	template <_STD size_t N>
	static _STD array<_STD shared_ptr<Buffer>, N> createBuffers() {
		_STD array<_STD reference_wrapper<Buffer>, N> buffers{};
		_STD array<u32, N> objects{};
		glCreateBuffers(N, objects.data());
		for (size_t i = 0; i < N; i++)
			buffers[i] = _STD make_shared<Buffer>({objects[i]});
		return buffers;
	}

	Buffer(Buffer const &) = delete;
	Buffer(Buffer &&) = delete;
	Buffer& operator=(Buffer const& p) = delete;
	Buffer& operator=(Buffer&& p) = delete;

	void   bind() const;
	void unbind() const;

	void setLabel(_STD string const& p_label) const;

	void allocStorage(_STD size_t const size, void const *data, gl::BufferStorageMask flags) const {
		glNamedBufferStorage(buffer_object_, static_cast<GLsizeiptr>(size), data, static_cast<GLbitfield>(flags));
	}

	_NODISCARD _STD size_t size() const;
	_NODISCARD bool immutable() const;

	// upload full data 
	void upload(_STD size_t const size, void const *data, gl::BufferUsageARB usage) const {
		glNamedBufferData(buffer_object_, static_cast<GLsizeiptr>(size), data, static_cast<GLenum>(usage));
	}

	// Upload sub data
	void update(_STD size_t const size, i64 const offset, void const *data) const {
		glNamedBufferSubData(buffer_object_, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
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

	friend class VertexArray;
};

enum class EComponentType : _STD uint8_t {
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

class VertexArray {
public:
	gl::PrimitiveType primitive_type = gl::PrimitiveType::Triangles;
	gl::DrawElementsType draw_elements_type = gl::DrawElementsType::UnsignedByte;
	_STD size_t elements_count = 0;
	
private:
	inline static u32 bound_object_ = 0xFFFFFFFFu;
	u32 vertex_array_object_;
	bool is_deleted_;
public:
	VertexArray() : vertex_array_object_(0), is_deleted_(false) {
		glCreateVertexArrays(1, &vertex_array_object_);
		gpuDebugf("Vertex Array #%u has been born", vertex_array_object_);
	}

	~VertexArray() {
		if (!is_deleted_) {
			gpuDebugf("Vertex Array #%u destroyed", vertex_array_object_);
			glDeleteVertexArrays(1, &vertex_array_object_);
		}
	}
	
	VertexArray(VertexArray const &) = delete;
	//VertexArray(VertexArray&&) = delete;
	VertexArray& operator=(VertexArray const& p) = delete;
	//VertexArray& operator=(VertexArray&& p) = delete;

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

	void setLabel(_STD string_view const p_label) const;

	void enableAttribute(u32 const p_bindingindex) const;

	void setAttribute(VertexAttribute_t const &p_attrib) const;

	void setVertexBuffer(u32 const p_bindingindex, Buffer const &buffer, i32 const p_stride, i64 const p_offset = 0) const {
		glVertexArrayVertexBuffer(vertex_array_object_, p_bindingindex, buffer.buffer_object_, p_offset, p_stride);
	}

	void setElementBuffer(Buffer const &buffer) const {
		glVertexArrayElementBuffer(vertex_array_object_, buffer.buffer_object_);
	}

	_NODISCARD bool bound() const {
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
		glDrawElements(static_cast<GLenum>(primitive_type), static_cast<GLsizei>(elements_count), static_cast<GLenum>(draw_elements_type), nullptr);
	}
};

class Renderbuffer {
	u32 renderbuffer_object_;
public:
	Renderbuffer();
	~Renderbuffer();
	Renderbuffer(Renderbuffer const &) = delete;
	Renderbuffer &operator=(Renderbuffer const &p) = delete;

	void allocateStorage(glm::ivec2 const &size, gl::InternalFormat internalFormat) const;
	void allocateStorageMultisample(glm::ivec2 const &size, i32 samples, gl::InternalFormat internalFormat) const;

	friend class Framebuffer;
};

class Framebuffer {
	static u32 bound_framebuffer_;
	static u32 bound_draw_framebuffer_;
	static u32 bound_read_framebuffer_;
public:
	u32 framebuffer_object_;
	
	Framebuffer();
	Framebuffer(u32 index);
	~Framebuffer();
	Framebuffer(Framebuffer const &) = delete;
	Framebuffer &operator=(Framebuffer const &p) = delete;
	void bind(gl::FramebufferTarget target = gl::FramebufferTarget::Framebuffer) const;
	void unbind(gl::FramebufferTarget target = gl::FramebufferTarget::Framebuffer) const;
	void setLabel(_STD string_view const p_label) const;
	void attachTexture(gl::FramebufferAttachment attachment, Texture const &texture, i32 level = 0) const;
	void attachRenderbuffer(Renderbuffer const &renderbuffer, gl::FramebufferAttachment attachment = gl::FramebufferAttachment::DepthStencilAttachment) const;
	void setDrawBuffers(_STD vector<gl::ColorBuffer> const &buffers) const;
	_NODISCARD gl::FramebufferStatus status() const;

	void blit(Framebuffer const &dest, glm::ivec4 const &src, glm::ivec4 const &dst, gl::bitfield_t mask, gl::BlitFramebufferFilter filter) const;
};

extern Framebuffer default_framebuffer;
constexpr gl::int32_t model_matrix_location = 0;

enum class RenderPassType {
	Normal,
	Shadow
};

struct RenderPassInfo {
	RenderPassType pass;
	bool bind_albedo_texture;
	bool bind_normal_texture;
	bool bind_orm_texture;
};

extern void APIENTRY open_gl_debug_proc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam);