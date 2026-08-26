#include "forward.hpp"

#include <fstream>
#include <glm/gtx/string_cast.hpp>

#include "ecs/transform.h"
#include "ecs/3d/editor/editor_camera.hpp"
#include "ecs/core/scene_tree.hpp"
#include "gpu/driver.hpp"
#include "gpu/graphics.hpp"
#include "gpu/lighting.hpp"
#include "gpu/mesh.hpp"
#include "gpu/window.hpp"

ForwardRenderer::ForwardRenderer(SharedPtr<Window> const &window) : IRenderer(window), window_(window) {
	GraphicsBackend* driver = GraphicsDriver::get();
	
	const BufferDescriptor scene_data_desc{
		.label = "Scene Data Buffer",
		.size = sizeof(SceneData),
		.usage = gfx::BufferUsage::eShaderDeviceAddress,
		.memory_usage = gfx::MemoryUsage::eAuto,
		.allocation_hints = gfx::AllocationHint::eHostSequentialWrite | gfx::AllocationHint::eAllowTransferInstead | gfx::AllocationHint::eMapped
	};
	
	scene_data_rid_ = driver->CreateBuffer(scene_data_desc);
	scene_data_mapped_address_ = static_cast<SceneData*>(driver->GetMappedData(scene_data_rid_));
	
	const BufferDescriptor culled_data_desc{
		.label = "Culled Data Buffer",
		.size = sizeof(CulledData),
		.usage = gfx::BufferUsage::eShaderDeviceAddress,
		.memory_usage = gfx::MemoryUsage::ePreferHost,
		.allocation_hints = gfx::AllocationHint::eHostSequentialWrite | gfx::AllocationHint::eMapped
	};
	
	culled_data_rid_ = driver->CreateBuffer(culled_data_desc);
	culled_data_mapped_address_ = static_cast<CulledData*>(driver->GetMappedData(culled_data_rid_));
	
	std::ifstream file("shaders/vulkan/mesh_shader_standard.spv", std::ios::binary | std::ios::ate);
	const uint32_t code_size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> code(code_size);
	file.read(code.data(), code_size);

	const SpirvDescriptor spirv_desc{
		.code_size = code_size,
		.code = (const uint32_t*)code.data()
	};
	
	shader = driver->CreateShader(spirv_desc);

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
	
	bind_group_layout = driver->CreateBindGroupLayout(bind_group_layout_desc);
	
	constexpr auto push_constant_size = sizeof(float4x4) * 1 + sizeof(GpuDeviceAddress) * 6 + sizeof(uint32_t);

	const PipelineLayoutDescriptor pipeline_layout_desc{
		.bind_group_layouts = {
			bind_group_layout
		},
		.push_constants = {
			PushConstantRangeDescriptor{
				.visibility = gfx::ShaderStage::eTask | gfx::ShaderStage::eMesh | gfx::ShaderStage::eFragment,
				.offset = 0,
				.size = push_constant_size
			}
		}
	};
	
	pipeline_layout = driver->CreatePipelineLayout(pipeline_layout_desc);

	const GraphicsPipelineDescriptor pipeline_descriptor{
		.layout = pipeline_layout,
		.stages = {
			GraphicsPipelineStageDescriptor{
				.shader = shader,
				.stage = gfx::ShaderStage::eTask,
				.entry_point = "taskMain"
			},
			GraphicsPipelineStageDescriptor{
				.shader = shader,
				.stage = gfx::ShaderStage::eMesh,
				.entry_point = "meshMain"
			},
			GraphicsPipelineStageDescriptor{
				.shader = shader,
				.stage = gfx::ShaderStage::eFragment,
				.entry_point = "fragmentMain"
			}
		},
		.rendering = {
			.color_formats = { driver->GetSurfaceColorFormat(window_->surface()) },
			.depth_format = gfx::Format::eDepth32SfloatStencil8Uint
		},
		.vertex_input = {},
		.input_assembly = {},
		.viewport = {
			.viewports = {
				Viewport{
					.x = 0.0f,
					.y = 0.0f,
					.width = (float)window_->size().x,
					.height = (float)window_->size().y,
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
						.width = (u32)window_->size().x,
						.height = (u32)window_->size().y
					}
				}
			}
		},
		.rasterization = {
			.cull_mode = gfx::CullMode::eFront,
			.front_face = gfx::FrontFace::eCounterClockwise
		},
		.multisample = {
			.rasterization_samples = gfx::SampleCount::e1
		},
		.depth_stencil = {
			.depth_test  = true,
			.depth_write = true,
			.depth_bounds_test = false,
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
	
	pipeline = driver->CreateGraphicsPipeline(pipeline_descriptor);
	
	//camera = window_->sceneTree()->createEntity();
	//editor_camera_ = &window_->sceneTree()->entity(camera)->component<EditorCamera3D>();
}

Result<> ForwardRenderer::resize(ivec2) {
	return OK;
}

Result<> ForwardRenderer::render() {
	
	GraphicsBackend* driver = GraphicsDriver::get();

	const Camera3D* current_camera = Camera3D::currentCameraEntity();

	const float4x4 view = current_camera->viewMatrix();
	const float4x4 proj = current_camera->projectionMatrix();
	const float4x4 proj_view = current_camera->projectionViewMatrix();
	
	const float4 camera_position(current_camera->entity()->component<Transform>().translation, 1.0f);
	
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
	
	scene_data_mapped_address_->point_lights = driver->GetBufferVirtualAddress(lighting_system->point_light_buffer_);
	scene_data_mapped_address_->spot_lights = driver->GetBufferVirtualAddress(lighting_system->spot_light_buffer_);
	
	scene_data_mapped_address_->delta_time = (float)window_->time() - scene_data_mapped_address_->time;
	scene_data_mapped_address_->time = (float)window_->time();
	
	timer_ += scene_data_mapped_address_->delta_time;
	
	if (timer_ > 0.5) {
		timer_ = 0.0;
		culled_data_ = *culled_data_mapped_address_;

		const String titleDebug = std::to_string(culled_data_.totalCulled); //glm::to_string(proj_view);
		window_->setTitle(titleDebug);
	}
	
	//std::memcpy(scene_data_mapped_address_, &scene_data_, sizeof(SceneData));
	std::memset(culled_data_mapped_address_, 0, sizeof(CulledData));
	
	const RID surface = window_->surface();
	const RID command_rid = driver->Begin(surface);
	const RID active_image = driver->GetActiveImage(surface);
	const Vector start_transition = {
		ImageTransitionDescriptor{
			.image = active_image,
			.src = ImageTransitionStateDescriptor{
				.layout = gfx::ImageLayout::eUndefined,
				.access = gfx::Access::eNone,
				.stage = gfx::PipelineStage::eColorAttachmentOutput
			},
			.dst = ImageTransitionStateDescriptor{
				.layout = gfx::ImageLayout::eAttachmentOptimal,
				.access = gfx::Access::eColorAttachmentWrite | gfx::Access::eColorAttachmentRead,
				.stage = gfx::PipelineStage::eColorAttachmentOutput
			},
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = BitFlag(gfx::Aspect::eColor)
			}
		},
		ImageTransitionDescriptor{
			.image = window_->depthImage(),
			.src = ImageTransitionStateDescriptor{
				.layout = gfx::ImageLayout::eUndefined,
				.access = gfx::Access::eDepthStencilAttachmentWrite,
				.stage = gfx::PipelineStage::eLateFragmentTests
			},
			.dst = ImageTransitionStateDescriptor{
				.layout = gfx::ImageLayout::eAttachmentOptimal,
				.access = gfx::Access::eDepthStencilAttachmentWrite,
				.stage = gfx::PipelineStage::eEarlyFragmentTests
			},
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = BitFlag(gfx::Aspect::eDepth) | gfx::Aspect::eStencil
			}
		}
	};
	
	driver->Transition(command_rid, start_transition);

	sceneTree()->initiateRenderSetup({
		.pass = RenderPassType::Normal,
		.scene_data = scene_data_mapped_address_,
		.material_bind_group_layout = bind_group_layout,
		.pipeline_layout = pipeline_layout,
		.pipeline = pipeline,
		.cmd = command_rid
	});

	driver->BeginRendering(surface, command_rid, pipeline, window_->depthImageView());

	const GpuDeviceAddress addresses[] = { driver->GetBufferVirtualAddress(scene_data_rid_), driver->GetBufferVirtualAddress(culled_data_rid_) };
	
	driver->PushConstants(command_rid, pipeline_layout, PushConstantRangeDescriptor{
		.visibility = gfx::ShaderStage::eTask | gfx::ShaderStage::eMesh | gfx::ShaderStage::eFragment,
		.offset = sizeof(float4x4),
		.size = sizeof(GpuDeviceAddress) * 2
	}, addresses);
	
	sceneTree()->initiateDraw({
		.pass = RenderPassType::Normal,
		.scene_data = scene_data_mapped_address_,
		.material_bind_group_layout = bind_group_layout,
		.pipeline_layout = pipeline_layout,
		.pipeline = pipeline,
		.cmd = command_rid
	});
	
	driver->FinishRendering(command_rid);
	
	const Vector finish_transition = {
		ImageTransitionDescriptor{
			.image = active_image,
			.src = {
				.layout = gfx::ImageLayout::eAttachmentOptimal,
				.access = BitFlag(gfx::Access::eColorAttachmentWrite) | BitFlag(gfx::Access::eColorAttachmentRead),
				.stage = BitFlag(gfx::PipelineStage::eColorAttachmentOutput)
			},
			.dst = {
				.layout = gfx::ImageLayout::ePresent,
				.access = BitFlag(gfx::Access::eNone),
				.stage = BitFlag(gfx::PipelineStage::eColorAttachmentOutput)
			},
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = BitFlag(gfx::Aspect::eColor)
			}
		}
	};
	
	driver->Transition(command_rid, finish_transition);
	driver->Finish(command_rid);

	driver->Submit(surface, command_rid);
	driver->Present(surface);
	
	render_semaphore.release();
	return OK;
}
SharedPtr<SceneTree> ForwardRenderer::sceneTree() const {
	return window_->sceneTree();
}

RendererType ForwardRenderer::rendererType() const {
	return RendererType::FORWARD;
}

RID ForwardRenderer::primaryBindGroupLayout() const {
	return bind_group_layout;
}

void ForwardRenderer::requestNewFrame() {
	render();
	//if (render_semaphore.try_acquire_for(std::chrono::milliseconds(0))) {
	//	render_future = std::async([&] {
	//	});
	//}
}

const SceneData & ForwardRenderer::sceneData() const {
	return scene_data_;
}

SceneData & ForwardRenderer::sceneDataMut() {
	return scene_data_;
}

RID ForwardRenderer::sceneDataRid() const {
	return scene_data_rid_;
}

void ForwardRenderer::dispose() {
	is_disposed_ = true;
	
	GraphicsBackend*driver=GraphicsDriver::get();
	driver->DestroyBuffer(scene_data_rid_);
	driver->DestroyBuffer(culled_data_rid_);
	driver->DestroyPipeline(pipeline);
	driver->DestroyPipelineLayout(pipeline_layout);
	driver->DestroyBindGroupLayout(bind_group_layout);
}

bool ForwardRenderer::disposed() const {
	return is_disposed_;
}
