#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <thread>

#include "geometry.hpp"
#include "types.hpp"
#include "opengl_enums.hpp"
#include "engine/disposable.hpp"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"

namespace helix::render {
	
class Buffer;
class Texture;
class Camera3D;
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

	namespace debug {
		extern bool check(char const *where, _STD size_t const line);
	}

#if defined(_DEBUG) || (defined(GPU_DEBUG) && GPU_DEBUG > 0)
#define gpu_check (::helix::render::debug::check(__FILE__, __LINE__))
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

	class SceneTree;

	class Window : public IDisposable {
		SharedPtr<SceneTree> scene_tree_;
	public:
		GLFWwindow *window;
		Window();
		Window(
			ivec2 const &p_startingSize,
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

		_NODISCARD ivec2 getSize() const;
		void setSize(ivec2 const& size) const;

		_NODISCARD ivec4 viewport() const;

		_NODISCARD bool shouldClose() const;

		void setSceneTree(SharedPtr<SceneTree> const& scene_tree);
		_NODISCARD SharedPtr<SceneTree> const& sceneTree() const;

		void hide() const;
		void show() const;
		void setVisible(bool visible) const;
		_NODISCARD bool visible() const;

		void setFramebufferSizeCallback(GLFWframebuffersizefun fun) const;

		void makeContextCurrent() const;
		void swapBuffers() const;
	
		void dispose() override;
		[[nodiscard]] bool disposed() const override;
	};

	// Program

	class Shader;

	//< Abstraction of a ShaderProgram
	class IProgram {
	public:
		virtual ~IProgram() = default;

		void link() const;
		void use() const;
		void integrityCheck();

		void dispatchCompute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z) const;
		void dispatchComputeIndirect(GLintptr indirect_offset) const;
	
		_NODISCARD bool inUse() const;
		_NODISCARD i32 uniformLocation(_STD string const &p_name) const;

		void setUniform(i32 uniform, mat4 const &p_matrix, bool transposed = false) const;
		void setUniform(i32 uniform, mat3 const &p_matrix, bool transposed = false) const;
		void setUniform(i32 uniform, glm::mat2 const &p_matrix, bool transposed = false) const;

		void setUniform(_STD string const &uniform, mat4 const &p_matrix, bool transposed = false) const;
		void setUniform(_STD string const &uniform, mat3 const &p_matrix, bool transposed = false) const;
		void setUniform(_STD string const &uniform, glm::mat2 const &p_matrix, bool transposed = false) const;

		void setUniform(i32 uniform, _STD vector<mat4> const &p_matrices, bool transposed = false) const;
		void setUniform(i32 uniform, _STD vector<mat3> const &p_matrices, bool transposed = false) const;
		void setUniform(i32 uniform, _STD vector<glm::mat2> const &p_matrices, bool transposed = false) const;

		void setUniform(_STD string const &uniform, _STD vector<mat4> const &p_matrices, bool transposed = false) const;
		void setUniform(_STD string const &uniform, _STD vector<mat3> const &p_matrices, bool transposed = false) const;
		void setUniform(_STD string const &uniform, _STD vector<glm::mat2> const &p_matrices, bool transposed = false) const;

		void setUniform(i32 uniform, vec4 const &p_vector) const;
		void setUniform(i32 uniform, vec3 const &p_vector) const;
		void setUniform(i32 uniform, vec2 const &p_vector) const;

		void setUniform(_STD string const &uniform, vec4 const &p_vector) const;
		void setUniform(_STD string const &uniform, vec3 const &p_vector) const;
		void setUniform(_STD string const &uniform, vec2 const &p_vector) const;

		void setUniform(i32 uniform, _STD vector<vec4> const &p_vectors) const;
		void setUniform(i32 uniform, _STD vector<vec3> const &p_vectors) const;
		void setUniform(i32 uniform, _STD vector<vec2> const &p_vectors) const;

		void setUniform(_STD string const &uniform, _STD vector<vec4> const &p_vectors) const;
		void setUniform(_STD string const &uniform, _STD vector<vec3> const &p_vectors) const;
		void setUniform(_STD string const &uniform, _STD vector<vec2> const &p_vectors) const;

		void setUniform(i32 uniform, ivec4 const &p_vector) const;
		void setUniform(i32 uniform, ivec3 const &p_vector) const;
		void setUniform(i32 uniform, ivec2 const &p_vector) const;
	
		void setUniform(_STD string const &uniform, ivec4 const &p_vector) const;
		void setUniform(_STD string const &uniform, ivec3 const &p_vector) const;
		void setUniform(_STD string const &uniform, ivec2 const &p_vector) const;

		void setUniform(i32 uniform, _STD vector<ivec4> const &p_vectors) const;
		void setUniform(i32 uniform, _STD vector<ivec3> const &p_vectors) const;
		void setUniform(i32 uniform, _STD vector<ivec2> const &p_vectors) const;

		void setUniform(_STD string const &uniform, _STD vector<ivec4> const &p_vectors) const;
		void setUniform(_STD string const &uniform, _STD vector<ivec3> const &p_vectors) const;
		void setUniform(_STD string const &uniform, _STD vector<ivec2> const &p_vectors) const;

		void setUniform(i32 uniform, glm::bvec4 const &p_vector) const;
		void setUniform(i32 uniform, glm::bvec3 const &p_vector) const;
		void setUniform(i32 uniform, glm::bvec2 const &p_vector) const;

		void setUniform(_STD string const &uniform, glm::bvec4 const &p_vector) const;
		void setUniform(_STD string const &uniform, glm::bvec3 const &p_vector) const;
		void setUniform(_STD string const &uniform, glm::bvec2 const &p_vector) const;
	
		void setUniform(i32 uniform, i32 value) const;
		void setUniform(i32 uniform, u32 value) const;
		void setUniform(i32 uniform, i64 value) const;
		void setUniform(i32 uniform, u64 value) const;
		void setUniform(i32 uniform, f32 value) const;

		void setUniform(_STD string const &uniform, i32 value) const;
		void setUniform(_STD string const &uniform, u32 value) const;
		void setUniform(_STD string const &uniform, i64 value) const;
		void setUniform(_STD string const &uniform, u64 value) const;
		void setUniform(_STD string const &uniform, f32 value) const;
	};

	class Program : public IDisposable {
		inline static u32 program_in_use_ = 0xFFFFFFFFu;
		u32 program_object_;

		// PLEASE SWITCH TO BOXES! 
		_STD vector<_STD reference_wrapper<Shader>> shaders_;
		Vec<Box<Shader>> owned_shaders_;
	
	public:
		Program();
		Program(std::string_view compute);
		Program(std::string_view vert, std::string_view frag);
		Program(std::string_view vert, std::string_view geom, std::string_view frag);
		~Program() override;

		Program(Program const& program) = delete;
		Program(Program&& program) = delete;
		Program& operator=(Program const& program) = delete;
		Program& operator=(Program&& program) = delete;
	
		void attach(Shader &p_shader);
		void attach(Box<Shader> p_shader);
		void setLabel(_STD string_view p_label) const;
	
		void link() const;

		[[nodiscard]] std::string linkLog() const;
		[[nodiscard]] bool linkStatus() const;
	
		void use() const;
		void integrityCheck();

		void dispatchCompute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z) const;
		void dispatchComputeIndirect(GLintptr indirect_offset) const;
	
		_NODISCARD bool inUse() const;
		_NODISCARD i32 uniformLocation(_STD string const &p_name) const;

		void setUniform(i32 uniform, mat4 const &p_matrix, bool transposed = false) const;
		void setUniform(i32 uniform, mat3 const &p_matrix, bool transposed = false) const;
		void setUniform(i32 uniform, glm::mat2 const &p_matrix, bool transposed = false) const;

		void setUniform(_STD string const &uniform, mat4 const &p_matrix, bool transposed = false) const;
		void setUniform(_STD string const &uniform, mat3 const &p_matrix, bool transposed = false) const;
		void setUniform(_STD string const &uniform, glm::mat2 const &p_matrix, bool transposed = false) const;

		void setUniform(i32 uniform, _STD vector<mat4> const &p_matrices, bool transposed = false) const;
		void setUniform(i32 uniform, _STD vector<mat3> const &p_matrices, bool transposed = false) const;
		void setUniform(i32 uniform, _STD vector<glm::mat2> const &p_matrices, bool transposed = false) const;

		void setUniform(_STD string const &uniform, _STD vector<mat4> const &p_matrices, bool transposed = false) const;
		void setUniform(_STD string const &uniform, _STD vector<mat3> const &p_matrices, bool transposed = false) const;
		void setUniform(_STD string const &uniform, _STD vector<glm::mat2> const &p_matrices, bool transposed = false) const;

		void setUniform(i32 uniform, vec4 const &p_vector) const;
		void setUniform(i32 uniform, vec3 const &p_vector) const;
		void setUniform(i32 uniform, vec2 const &p_vector) const;

		void setUniform(_STD string const &uniform, vec4 const &p_vector) const;
		void setUniform(_STD string const &uniform, vec3 const &p_vector) const;
		void setUniform(_STD string const &uniform, vec2 const &p_vector) const;

		void setUniform(i32 uniform, _STD vector<vec4> const &p_vectors) const;
		void setUniform(i32 uniform, _STD vector<vec3> const &p_vectors) const;
		void setUniform(i32 uniform, _STD vector<vec2> const &p_vectors) const;

		void setUniform(_STD string const &uniform, _STD vector<vec4> const &p_vectors) const;
		void setUniform(_STD string const &uniform, _STD vector<vec3> const &p_vectors) const;
		void setUniform(_STD string const &uniform, _STD vector<vec2> const &p_vectors) const;

		void setUniform(i32 uniform, ivec4 const &p_vector) const;
		void setUniform(i32 uniform, ivec3 const &p_vector) const;
		void setUniform(i32 uniform, ivec2 const &p_vector) const;
	
		void setUniform(_STD string const &uniform, ivec4 const &p_vector) const;
		void setUniform(_STD string const &uniform, ivec3 const &p_vector) const;
		void setUniform(_STD string const &uniform, ivec2 const &p_vector) const;

		void setUniform(i32 uniform, _STD vector<ivec4> const &p_vectors) const;
		void setUniform(i32 uniform, _STD vector<ivec3> const &p_vectors) const;
		void setUniform(i32 uniform, _STD vector<ivec2> const &p_vectors) const;

		void setUniform(_STD string const &uniform, _STD vector<ivec4> const &p_vectors) const;
		void setUniform(_STD string const &uniform, _STD vector<ivec3> const &p_vectors) const;
		void setUniform(_STD string const &uniform, _STD vector<ivec2> const &p_vectors) const;

		void setUniform(i32 uniform, glm::bvec4 const &p_vector) const;
		void setUniform(i32 uniform, glm::bvec3 const &p_vector) const;
		void setUniform(i32 uniform, glm::bvec2 const &p_vector) const;

		void setUniform(_STD string const &uniform, glm::bvec4 const &p_vector) const;
		void setUniform(_STD string const &uniform, glm::bvec3 const &p_vector) const;
		void setUniform(_STD string const &uniform, glm::bvec2 const &p_vector) const;
	
		void setUniform(i32 uniform, i32 value) const;
		void setUniform(i32 uniform, u32 value) const;
		void setUniform(i32 uniform, i64 value) const;
		void setUniform(i32 uniform, u64 value) const;
		void setUniform(i32 uniform, f32 value) const;

		void setUniform(_STD string const &uniform, i32 value) const;
		void setUniform(_STD string const &uniform, u32 value) const;
		void setUniform(_STD string const &uniform, i64 value) const;
		void setUniform(_STD string const &uniform, u64 value) const;
		void setUniform(_STD string const &uniform, f32 value) const;
	
		void dispose() override;
		[[nodiscard]] bool disposed() const override;
	};
	// Shader

	class Shader : public IDisposable {
	public:
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
		void dispose() override;
		[[nodiscard]] bool disposed() const override;

		friend class Program;
	};

	// Texture
	class Framebuffer;

	// Buffer
	class VertexArray;

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

	class VertexArray : public IDisposable{
	public:
		gl::PrimitiveType primitive_type = gl::PrimitiveType::Triangles;
		gl::DrawElementsType draw_elements_type = gl::DrawElementsType::UnsignedByte;
		_STD size_t elements_count = 0;
		_STD size_t offset_of_elements = 0;
		size_t vertex_buffer_count = 0;
	
	private:
		inline static u32 bound_object_ = 0xFFFFFFFFu;
		u32 vertex_array_object_;
		bool is_deleted_;
	public:
		VertexArray();
		~VertexArray() override;
		VertexArray(VertexArray const &) = delete;
		VertexArray& operator=(VertexArray const& p) = delete;
	
		void bind() const;
		void unbind() const;
		void setLabel(_STD string_view const p_label) const;
		void enableAttribute(u32 const p_bindingindex) const;
		void setAttribute(VertexAttribute_t const &p_attrib) const;
		void setVertexBuffer(u32 const p_bindingindex, Buffer const &buffer, i32 const p_stride, i64 const p_offset = 0) const;
		void setElementBuffer(Buffer const &buffer) const;
		_NODISCARD bool bound() const;
		void drawArrays(gl::PrimitiveType prim = gl::PrimitiveType::Triangles, i32 const first = 0, i32 const count = 0) const;
		void drawArraysInstanced(gl::PrimitiveType prim, i32 const first, i32 const count, i32 const instances) const;
		void drawElements(gl::PrimitiveType prim = gl::PrimitiveType::Triangles, gl::DrawElementsType elem = gl::DrawElementsType::UnsignedByte, i32 const count = 0) const;
		void draw() const;
		void dispose() override;
		[[nodiscard]] bool disposed() const override;
	};

	enum class RenderPassType {
		Normal,
		Shadow
	};

	/**
	 * @brief A set of bindings to tell the material where to send the material's information.
	 *
	 * To properly send a material's information to a shader, you must supply a bridge.
	 * This object dictates what shader parameters correspond to what material properties.
	 * This is because different shaders may have different parameter names for the same material property,
	 * and some shaders may not even use certain material properties at all. The bridge also allows for more
	 * efficient rendering by avoiding unnecessary shader parameter updates for properties that are not used by the shader
	 */
	struct MaterialBridge {
		i32 diffuse_texture_unit = -1; //< Location for the diffuse texture is not necessary, as the shader will bind the unit to the shader correctly. Use `diffuse_texture` if using ARB_bindless_textures.
		i32 diffuse_texture = -1; //< Uses ARB_bindless_textures
		i32 diffuse_texture_is_used = -1;
		i32 diffuse_color_modulation = -1;

		i32 orm_texture_unit = -1;
		i32 orm_texture = -1;
		i32 orm_texture_is_used = -1;

		i32 occlusion_is_used = -1;
		i32 occlusion_strength = -1;

		i32 roughness_bias = -1; //< Bias = Offset
		i32 roughness_scale = -1; //< Scale multiplies the roughness channel of the ORM texture.

		i32 metallic_bias = -1; //< Bias = Offset
		i32 metallic_scale = -1; //< Scale multiplies the metallic channel of the ORM texture.

		i32 normal_texture_unit = -1;
		i32 normal_texture = -1;
		i32 normal_texture_is_used = -1;
		i32 normal_texture_strength = -1;

		i32 emissive_texture_unit = -1;
		i32 emissive_texture = -1;
		i32 emissive_texture_is_used = -1;

		i32 emissive_color_modulation = -1; //< Modulates the emissive color channel of the shader, allowing for dynamic changes to the emissive color without needing to update the texture.
		i32 emissive_blend_mode = -1; //< Generally: 0: Emission = (Texture * Modulation), 1: Emission = (Texture + Modulation)

		i32 emissive_bias = -1; //<
		i32 emissive_scale = -1;
	};
	struct RenderPassInfo {
		RenderPassType pass;
		Frustum camera;
		i32 model_matrix_location = -1;
		i32 inverse_model_matrix_location = -1;
		i32 debug_hovered_location = -1;
		i32 view_matrix_location = -1;
		i32 projection_matrix_location = -1;
		i32 inverse_view_matrix_location = -1;
		i32 inverse_projection_matrix_location = -1;
		bool bind_model_matrix = true;
		bool bind_debug_hovered = false;
		bool bind_albedo_texture = false;
		bool bind_normal_texture = false;
		bool bind_orm_texture = false;
		bool bind_object_id = false;
		bool frustum_culling = false;
		bool render_sky = false;
		bool cull = false;
		gl::TriangleFace cull_face = gl::TriangleFace::Back;
		Optional<i32> bind_time;
		ivec4 viewport = ivec4(0, 0, 1, 1);
		Program *shader_program;
		struct RenderPassInfo_BlendControl {
			bool enabled = false;
			Optional<gl::BlendingFactor> src;
			Optional<gl::BlendingFactor> dst;
		} blend;
		struct RenderPassInfo_DepthControl {
			bool depth_test = true;
			gl::DepthFunction func = gl::DepthFunction::Less;
			vec2 range = vec2(0.0f, 1.0f);
		} depth;
		struct CascadedShadowMapping {
			bool bind_buffer = false;
			i32 buffer_binding = 0;
			i32 bind_texture_unit = -1;
			i32 bind_texture_location = -1;
			i32 light_direction_location = -1;
			i32 far_plane_location = -1;
			i32 world_position_location = -1;
		} csm;
		MaterialBridge material_bridge;
	};

	extern void APIENTRY open_gl_debug_proc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam);
}