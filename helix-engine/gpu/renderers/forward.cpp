#include "forward.hpp"

#include <fstream>
#include <glm/gtx/string_cast.hpp>

#include "backends/imgui_impl_vulkan.h"
#include "ecs/transform.h"
#include "ecs/3d/editor/editor_camera.hpp"
#include "ecs/core/scene_tree.hpp"
#include "gpu/driver.hpp"
#include "gpu/graphics.hpp"
#include "gpu/lighting.hpp"
#include "gpu/mesh.hpp"
#include "gpu/window.hpp"
#include "gpu/backends/vulkan_backend.hpp"

ForwardRenderer::ForwardRenderer(SharedPtr<Window> const &window) : IRenderer(window), window_(window) {
	GraphicsBackend* driver = GraphicsDriver::get();
	
	const BufferDescriptor scene_data_desc{
		.label = "Scene Data Buffer",
		.size = sizeof(SceneData),
		.usage = gfx::BufferUsage::eShaderDeviceAddress,
		.memory_usage = gfx::MemoryUsage::eAuto,
		.allocation_hints = gfx::AllocationHint::eHostSequentialWrite | gfx::AllocationHint::eAllowTransferInstead | gfx::AllocationHint::eMapped
	};
	
	scene_data_rid_ = driver->create_buffer(scene_data_desc);
	scene_data_mapped_address_ = static_cast<SceneData*>(driver->get_mapped_data(scene_data_rid_));
	
	const BufferDescriptor culled_data_desc{
		.label = "Culled Data Buffer",
		.size = sizeof(CulledData),
		.usage = gfx::BufferUsage::eShaderDeviceAddress,
		.memory_usage = gfx::MemoryUsage::ePreferHost,
		.allocation_hints = gfx::AllocationHint::eHostSequentialWrite | gfx::AllocationHint::eMapped
	};
	
	culled_data_rid_ = driver->create_buffer(culled_data_desc);
	culled_data_mapped_address_ = static_cast<CulledData*>(driver->get_mapped_data(culled_data_rid_));
	
	std::ifstream file("shaders/vulkan/standard.spv", std::ios::binary | std::ios::ate);
	const uint32_t code_size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> code(code_size);
	file.read(code.data(), code_size);

	const SpirvDescriptor spirv_desc{
		.code_size = code_size,
		.code = (const uint32_t*)code.data()
	};
	
	shader = driver->create_shader(spirv_desc);

	const BindGroupLayoutDescriptor bind_group_layout_desc{
		.label = "ForwardRenderer Bind Group Layout",
		.entries = {
			BindGroupLayoutEntryDescriptor{
				.binding = 0,
				.visibility = gfx::ShaderStage::eFragment,
				.type = gfx::BindingType::eSampledImage,
				.count = 1
			},
			BindGroupLayoutEntryDescriptor{
				.binding = 1,
				.visibility = gfx::ShaderStage::eFragment,
				.type = gfx::BindingType::eSampledImage,
				.count = 1
			},
			BindGroupLayoutEntryDescriptor{
				.binding = 2,
				.visibility = gfx::ShaderStage::eFragment,
				.type = gfx::BindingType::eSampledImage,
				.count = 1
			},
			BindGroupLayoutEntryDescriptor{
				.binding = 3,
				.visibility = gfx::ShaderStage::eFragment,
				.type = gfx::BindingType::eSampler,
				.count = 1
			}
		}
	};
	
	bind_group_layout = driver->create_bind_group_layout(bind_group_layout_desc);
	
	constexpr size_t push_constant_size = sizeof(GpuDeviceAddress) * 2;

	const PipelineLayoutDescriptor pipeline_layout_desc{
		.bind_group_layouts = {
			bind_group_layout
		},
		.push_constants = {
			PushConstantRangeDescriptor{
				.visibility = gfx::ShaderStage::eVertex | gfx::ShaderStage::eFragment,
				.offset = 0,
				.size = push_constant_size
			}
		}
	};
	
	pipeline_layout = driver->create_pipeline_layout(pipeline_layout_desc);

	const GraphicsPipelineDescriptor pipeline_descriptor{
		.label = "ForwardRenderer Pipeline",
		.layout = pipeline_layout,
		.stages = {
			GraphicsPipelineStageDescriptor{
				.shader = shader,
				.stage = gfx::ShaderStage::eVertex,
				.entry_point = "main"
			},
			GraphicsPipelineStageDescriptor{
				.shader = shader,
				.stage = gfx::ShaderStage::eFragment,
				.entry_point = "main"
			}
		},
		.rendering = {
			.color_formats = { driver->get_surface_color_format(window_->get_surface()) },
			.depth_format = gfx::Format::eDepth32SfloatStencil8Uint
		},
		.vertex_input = Vertex::inputState(),
		.input_assembly = {
			.primitive_topology = gfx::PrimitiveTopology::eTriangleList,
			.primitive_restart_enable = false
		},
		.viewport = {
			.viewports = {
				Viewport{
					.x = 0.0f,
					.y = 0.0f,
					.width = (float)window_->get_size().x,
					.height = (float)window_->get_size().y,
					.min_depth = 0.0f,
					.max_depth = 1.0f
				}
			},
			.scissors = {
				Rect2D{
					.offset = {
						.x = 0,
						.y = 0 
					},
					.extent = {
						.width = (u32)window_->get_size().x,
						.height = (u32)window_->get_size().y
					}
				}
			}
		},
		.rasterization = {
			.cull_mode = gfx::CullMode::eNone,
			.front_face = gfx::FrontFace::eCounterClockwise
		},
		.multisample = {
			.rasterization_samples = gfx::SampleCount::e1
		},
		.depth_stencil = {
			.depth_test  = true,
			.depth_write = true,
			.depth_bounds_test = true,
			.stencil_test = false,
			.depth_compare_op = gfx::CompareOp::eLess,
			.front = {},
			.back = {},
			.min_depth_bounds = 0.0f,
			.max_depth_bounds = 1.0f
		},
		.blend = {
			.blend_enable = false
		},
		.dynamic_states = {
			gfx::DynamicState::eViewport,
			gfx::DynamicState::eScissor
		}
	};
	
	pipeline = driver->create_graphics_pipeline(pipeline_descriptor);
}

Result<> ForwardRenderer::resize(ivec2) {
	return OK;
}

Result<> ForwardRenderer::render() {
	
	GraphicsBackend* driver = GraphicsDriver::get();

	const Camera3D* current_camera = Camera3D::get_current_camera_entity();
	
	Camera3D* current_camera_mut = const_cast<Camera3D*>(current_camera);
	current_camera_mut->set_field_of_vision(glm::radians(89.0f));
	current_camera_mut->set_aspect_ratio((f32)window_->get_size().x / (f32)window_->get_size().y);
	current_camera_mut->set_far_plane(8.0f);
	current_camera_mut->set_near_plane(0.05f);
	current_camera_mut->refresh_matrices();
	
	const float4x4 view = current_camera->get_view();
	const float4x4 proj = current_camera->get_projection();
	const float4x4 proj_view = current_camera->get_projection_view();
	
	const float4 camera_position(current_camera->get_entity()->get_component<Transform>().translation, 1.0f);
	
	scene_data_mapped_address_->view = view;
	scene_data_mapped_address_->proj = proj;
	scene_data_mapped_address_->proj_view = proj_view;
	
	scene_data_mapped_address_->frozen_view = view;
	scene_data_mapped_address_->frozen_proj = proj;	
	scene_data_mapped_address_->frozen_proj_view = proj_view;
	
	scene_data_mapped_address_->frustum = Frustum(proj_view);
	scene_data_mapped_address_->frozen_frustum = Frustum(proj_view);
	
	scene_data_mapped_address_->camera_world_position = camera_position;
	scene_data_mapped_address_->frozen_camera_world_position = scene_data_mapped_address_->camera_world_position;

	const LightingSystem* lighting_system = LightingSystem::singleton();
	
	scene_data_mapped_address_->point_lights = driver->get_buffer_virtual_address(lighting_system->point_light_buffer_);
	scene_data_mapped_address_->spot_lights = driver->get_buffer_virtual_address(lighting_system->spot_light_buffer_);
	
	scene_data_mapped_address_->delta_time = (float)window_->get_time() - scene_data_mapped_address_->time;
	scene_data_mapped_address_->time = (float)window_->get_time();
	
	timer_ += scene_data_mapped_address_->delta_time;
	
	if (timer_ > 0.5) {
		timer_ = 0.0;
		culled_data_ = *culled_data_mapped_address_;

		const String titleDebug = std::to_string(culled_data_.totalCulled); //glm::to_string(proj_view);
		window_->set_title(titleDebug);
	}
	
	//std::memcpy(scene_data_mapped_address_, &scene_data_, sizeof(SceneData));
	std::memset(culled_data_mapped_address_, 0, sizeof(CulledData));
	
	const RID surface = window_->get_surface();
	const RID command_rid = driver->begin(surface);
	
	get_scene_tree()->init_render_setup({
		.pass = RenderPassType::Normal,
		.view = view,
		.projection = proj,
		.scene_data = scene_data_mapped_address_,
		.material_bind_group_layout = bind_group_layout,
		.pipeline_layout = pipeline_layout,
		.pipeline = pipeline,
		.cmd = command_rid
	});

	driver->begin_rendering(surface, command_rid, pipeline, window_->get_depth_image_view());

	const GpuDeviceAddress addresses[] = { driver->get_buffer_virtual_address(scene_data_rid_) };
	
	driver->push_constants(command_rid, pipeline_layout, PushConstantRangeDescriptor{
		.visibility = gfx::ShaderStage::eVertex | gfx::ShaderStage::eFragment,
		.offset = 0,
		.size = sizeof(GpuDeviceAddress)
	}, addresses);
	
	get_scene_tree()->init_draw({
		.pass = RenderPassType::Normal,
		.scene_data = scene_data_mapped_address_,
		.material_bind_group_layout = bind_group_layout,
		.pipeline_layout = pipeline_layout,
		.pipeline = pipeline,
		.cmd = command_rid
	});

	const VkGraphicsBackend* vk = dynamic_cast<VkGraphicsBackend*>(driver);
	
	driver->push_label(command_rid, "ImGui Render");
	ImGui::Render();
	//ImGui_ImplVulkan_Ren	derDrawData(ImGui::GetDrawData(), vk->GetCommandBuffer(command_rid), nullptr);
	driver->pop_label(command_rid);
	
	driver->finish_rendering(command_rid);
	driver->finish(command_rid);

	driver->submit(command_rid);
	driver->present(surface);
	
	render_semaphore.release();
	return OK;
}
SharedPtr<SceneTree> ForwardRenderer::get_scene_tree() const {
	return window_->get_scene_tree();
}

RendererType ForwardRenderer::get_renderer_type() const {
	return RendererType::FORWARD;
}

RID ForwardRenderer::get_primary_bind_group_layout() const {
	return bind_group_layout;
}

void ForwardRenderer::request_new_frame() {
	render();
	//if (render_semaphore.try_acquire_for(std::chrono::milliseconds(0))) {
	//	render_future = std::async([&] {
	//	});
	//}
}

const SceneData & ForwardRenderer::get_scene_data() const {
	return scene_data_;
}

SceneData & ForwardRenderer::get_scene_data_mutable() {
	return scene_data_;
}

RID ForwardRenderer::get_scene_data_rid() const {
	return scene_data_rid_;
}

void ForwardRenderer::dispose() {
	is_disposed_ = true;
	
	GraphicsBackend*driver=GraphicsDriver::get();
	driver->destroy_buffer(scene_data_rid_);
	driver->destroy_buffer(culled_data_rid_);
	driver->destroy_pipeline(pipeline);
	driver->destroy_pipeline_layout(pipeline_layout);
	driver->destroy_bind_group_layout(bind_group_layout);
}

bool ForwardRenderer::disposed() const {
	return is_disposed_;
}
