#include "forward.hpp"

#include <fstream>
#include <glm/gtx/string_cast.hpp>

#include "backends/imgui_impl_sdl2.h"
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
#define FRAMETIME_DEBUGGING
ForwardRenderer::ForwardRenderer(SharedPtr<Window> const &window) : IRenderer(window), window_(window) {
	IGpuDriver* driver = GraphicsSystem::get_driver();
	
	const BufferDescriptor scene_data_desc{
		.label = "Scene Data Buffer",
		.size = sizeof(SceneData) * 2,
		.usage = gfx::BufferUsage::eShaderDeviceAddress,
		.memory_usage = gfx::MemoryUsage::ePreferDevice,
		.allocation_hints = gfx::AllocationHint::eHostAccessRandom | gfx::AllocationHint::eMapped
	};
	
	scene_data_rid_ = driver->create_buffer(scene_data_desc);
	scene_data_mapped_address_ = static_cast<SceneData*>(driver->get_mapped_data(scene_data_rid_));
	
	const BufferDescriptor culled_data_desc{
		.label = "Culled Data Buffer",
		.size = sizeof(CulledData),
		.usage = gfx::BufferUsage::eShaderDeviceAddress,
		.memory_usage = gfx::MemoryUsage::ePreferHost,
		.allocation_hints = gfx::AllocationHint::eHostAccessRandom | gfx::AllocationHint::eMapped
	};
	
	culled_data_rid_ = driver->create_buffer(culled_data_desc);
	culled_data_mapped_address_ = static_cast<CulledData*>(driver->get_mapped_data(culled_data_rid_));
	
	std::ifstream file("shaders/vulkan/standard.spv", std::ios::binary | std::ios::ate);
	const uint32_t code_size = static_cast<u32>(file.tellg());
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
	
	const ViewportStateDescriptor viewport_state_descriptor{
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
		.scissors = { Rect2D{
			.offset = { .x = 0, .y = 0 },
			.extent = { .width = (u32)window_->get_size().x, .height = (u32)window_->get_size().y }
		} }
	};

	constexpr DepthStencilDescriptor depth_stencil_descriptor {
		.depth_test  = true,
		.depth_write = true,
		.depth_bounds_test = true,
		.stencil_test = false,
		.depth_compare_op = gfx::CompareOp::eLess,
		.front = {},
		.back = {},
		.min_depth_bounds = 0.0f,
		.max_depth_bounds = 1.0f
	};

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
		.vertex_input = Vertex::input_state(),
		.input_assembly = InputAssemblyDescriptor::topology(gfx::PrimitiveTopology::eTriangleList),
		.viewport = viewport_state_descriptor,
		.rasterization = {
			.cull_mode = gfx::CullMode::eFront,
			.front_face = gfx::FrontFace::eCounterClockwise
		},
		.multisample = {
			.rasterization_samples = gfx::SampleCount::e1
		},
		.depth_stencil = depth_stencil_descriptor,
		.blend = ColorBlendStateDescriptor::disabled(),
		.dynamic_states = {
			gfx::DynamicState::eViewport,
			gfx::DynamicState::eScissor,
			gfx::DynamicState::eDepthTestEnable,
			gfx::DynamicState::eDepthWriteEnable
		}
	};
	
	pipeline = driver->create_graphics_pipeline(pipeline_descriptor);
}

Result<> ForwardRenderer::resize(ivec2) {
	return OK;
}

Result<> ForwardRenderer::render() {
	
	IGpuDriver* driver = GraphicsSystem::get_driver();

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
	
	const Entity* camera_entity = current_camera->get_entity();
	const Transform& camera_transform = camera_entity->get_component<Transform>();
	const float4 camera_position = float4(camera_transform.translation, 1.0f);
	
	const RID surface = window_->get_surface();
	const RID command_rid = driver->begin(surface);
	const u32 frame_index = driver->get_frame_index(surface);
	
	if (camera_transform.dirty_[frame_index]) {
		scene_data_mapped_address_[frame_index].view = view;
		scene_data_mapped_address_[frame_index].proj = proj;
		scene_data_mapped_address_[frame_index].proj_view = proj_view;
		scene_data_mapped_address_[frame_index].frozen_view = view;
		scene_data_mapped_address_[frame_index].frozen_proj = proj;	
		scene_data_mapped_address_[frame_index].frozen_proj_view = proj_view;
		scene_data_mapped_address_[frame_index].frustum = Frustum(proj_view);
		scene_data_mapped_address_[frame_index].frozen_frustum = Frustum(proj_view);
		scene_data_mapped_address_[frame_index].camera_world_position = camera_position;
		scene_data_mapped_address_[frame_index].frozen_camera_world_position = scene_data_mapped_address_[frame_index].camera_world_position;
		camera_transform.dirty_[frame_index] = false;
	}
	
	const LightingSystem* lighting_system = LightingSystem::singleton();
	if (scene_data_mapped_address_[frame_index].point_lights != driver->get_buffer_virtual_address(lighting_system->point_light_buffer_))
		scene_data_mapped_address_[frame_index].point_lights = driver->get_buffer_virtual_address(lighting_system->point_light_buffer_);
	if (scene_data_mapped_address_[frame_index].spot_lights != driver->get_buffer_virtual_address(lighting_system->spot_light_buffer_))
		scene_data_mapped_address_[frame_index].spot_lights = driver->get_buffer_virtual_address(lighting_system->spot_light_buffer_);
	
	scene_data_mapped_address_[frame_index].delta_time = static_cast<float>(window_->get_time()) - last_time_;
	scene_data_mapped_address_[frame_index].time = (float)window_->get_time();
	
	last_time_ = static_cast<f32>(window_->get_time());
	
#ifdef FRAMETIME_DEBUGGING
	
	static float timings[128] = {};
	static int timing_offset = 0;
	static int averaging_offset = 0;
	static float average[2] = {0.0f, 0.0f};
	static float lowest[2] = {0.0f, 0.0f};
	static float highest[2] = {0.0f, 0.0f};
	timings[timing_offset] = scene_data_mapped_address_[frame_index].delta_time * 1000.0f;
	timing_offset = (timing_offset + 1) % 128;
	
	lowest[averaging_offset] = std::min(lowest[averaging_offset], timings[timing_offset]);
	highest[averaging_offset] = std::max(highest[averaging_offset], timings[timing_offset]);
	
	
	ImGui::Begin("Frame debug");
	ImGui::PlotLines("Frame time (ms)", timings, 128, timing_offset, nullptr, 5.0f, 50.0f, ImVec2(0, 80));
	
	int opposite_offset = (averaging_offset + 1) % 2;
	ImGui::Text("Frame time: %.2f ms", average[opposite_offset]);
	ImGui::Text("Lowest frame time: %.2f ms", lowest[opposite_offset]);
	ImGui::Text("Highest frame time: %.2f ms", highest[opposite_offset]);
	
	if (timing_offset % 128 == 0) {
		averaging_offset = (averaging_offset + 1) % 2;
		average[averaging_offset] = 0.0f;
		for (const float timing : timings) {
			average[averaging_offset] += timing;
		}
		average[averaging_offset] /= 128.0f;
		lowest[averaging_offset] = 10000.0f;
		highest[averaging_offset] = 0.0f;
	}
	
	ImGui::End();
#endif
	
	timer_ += scene_data_mapped_address_[frame_index].delta_time;
	
	if (timer_ > 0.5) {
		timer_ = 0.0;
		culled_data_ = *culled_data_mapped_address_;

		const String titleDebug = std::to_string(culled_data_.totalCulled); //glm::to_string(proj_view);
		window_->set_title(titleDebug);
	}
	
	//std::memcpy(scene_data_mapped_address_, &scene_data_, sizeof(SceneData));
	std::memset(culled_data_mapped_address_, 0, sizeof(CulledData));
	
	driver->flush_buffer(scene_data_rid_, ivec2(0, VK_WHOLE_SIZE));
	
	get_scene_tree()->init_render_setup({
		.pass = RenderPassType::Normal,
		.view = view,
		.projection = proj,
		.scene_data = scene_data_mapped_address_,
		.material_bind_group_layout = bind_group_layout,
		.pipeline_layout = pipeline_layout,
		.pipeline = pipeline,
		.cmd = command_rid,
		.frame_index = frame_index
	});

	driver->begin_rendering(surface, command_rid, pipeline, window_->get_depth_image_view());
	const GpuDeviceAddress addresses[] = { driver->get_buffer_virtual_address(scene_data_rid_) + sizeof(SceneData) * frame_index };
	
	driver->push_constants(command_rid, pipeline_layout, PushConstantRangeDescriptor{
		.visibility = gfx::ShaderStage::eVertex | gfx::ShaderStage::eFragment,
		.offset = 0,
		.size = sizeof(GpuDeviceAddress)
	}, addresses);
	driver->set_depth_test_enable(command_rid, true);
	driver->set_depth_write_enable(command_rid, true);
	get_scene_tree()->init_draw({
		.pass = RenderPassType::Normal,
		.view = view,
		.projection = proj,
		.scene_data = scene_data_mapped_address_,
		.material_bind_group_layout = bind_group_layout,
		.pipeline_layout = pipeline_layout,
		.pipeline = pipeline,
		.cmd = command_rid,
		.frame_index = frame_index
	});

	const VkGraphicsDriverBackend* vk = dynamic_cast<VkGraphicsDriverBackend*>(driver);
	driver->set_depth_test_enable(command_rid, false);
	driver->set_depth_write_enable(command_rid, false);
	
	driver->push_label(command_rid, "ImGui Render");
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vk->get_command_buffer(command_rid), nullptr);
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

SceneData & ForwardRenderer::get_scene_data() {
	return scene_data_;
}

RID ForwardRenderer::get_scene_data_rid() const {
	return scene_data_rid_;
}

void ForwardRenderer::dispose() {
	is_disposed_ = true;
	
	IGpuDriver*driver=GraphicsSystem::get_driver();
	driver->destroy_buffer(scene_data_rid_);
	driver->destroy_buffer(culled_data_rid_);
	driver->destroy_pipeline(pipeline);
	driver->destroy_pipeline_layout(pipeline_layout);
	driver->destroy_bind_group_layout(bind_group_layout);
}

bool ForwardRenderer::disposed() const {
	return is_disposed_;
}
