#include "forward.hpp"

#include <fstream>

#include "ecs/core/scene_tree.hpp"
#include "gpu/driver.hpp"
#include "gpu/graphics.hpp"
#include "gpu/mesh.hpp"
#include "gpu/window.hpp"

ForwardRenderer::ForwardRenderer(SharedPtr<Window> const &window) : IRenderer(window), window_(window) {
	GraphicsBackend* driver = GraphicsDriver::get();
	
	std::ifstream file("shaders/vulkan/standard.spv", std::ios::binary | std::ios::ate);
	const uint32_t code_size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> code(code_size);
	file.read(code.data(), code_size);

	const SpirvDescriptor spirv_desc{
		.code_size = code_size,
		.code = (const uint32_t*)code.data()
	};
	
	shader = driver->shader_create(spirv_desc);

	const BindGroupLayoutDescriptor bind_group_layout_desc{
		.label = "ForwardRenderer Bind Group Layout",
		.entries = {
			BindGroupLayoutEntryDescriptor{
				.binding = 0,
				.visibility = gfx::ShaderStage::eFragment,
				.type = gfx::BindingType::eImageSampler,
				.count = 1
			},
			BindGroupLayoutEntryDescriptor{
				.binding = 1,
				.visibility = gfx::ShaderStage::eFragment,
				.type = gfx::BindingType::eImageSampler,
				.count = 1
			},
			BindGroupLayoutEntryDescriptor{
				.binding = 2,
				.visibility = gfx::ShaderStage::eFragment,
				.type = gfx::BindingType::eImageSampler,
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
	
	bind_group_layout = driver->bind_group_layout_create(bind_group_layout_desc);

	const PipelineLayoutDescriptor pipeline_layout_desc{
		.bind_group_layouts = {
			bind_group_layout
		},
		.push_constants = {
			PushConstantRangeDescriptor{
				.visibility = gfx::ShaderStage::eVertex,
				.offset = 0,
				.size = sizeof(uintptr_t)
			}
		}
	};
	
	pipeline_layout = driver->pipeline_layout_create(pipeline_layout_desc);

	const GraphicsPipelineDescriptor pipeline_descriptor{
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
			.color_formats = { driver->surface_get_color_format(window_->surface()) },
			.depth_format = gfx::Format::eDepth32SfloatStencil8Uint
		},
		.vertex_input = {
			.bindings = {
				VertexInputBindingDescriptor{
					.binding = 0,
					.stride = sizeof(Vertex),
					.input_rate = gfx::InputRate::eVertex
				}
			},
			.attributes = {
				VertexInputAttributeDescriptor{
					.location = 0,
					.binding = 0,
					.format = gfx::Format::eRgb32Sfloat,
					.offset = offsetof(Vertex, position)
				},
				VertexInputAttributeDescriptor{
					.location = 1,
					.binding = 0,
					.format = gfx::Format::eRgb32Sfloat,
					.offset = offsetof(Vertex, normal)
				},
				VertexInputAttributeDescriptor{
					.location = 2,
					.binding = 0,
					.format = gfx::Format::eRgba32Sfloat,
					.offset = offsetof(Vertex, tangent)
				},
				VertexInputAttributeDescriptor{
					.location = 3,
					.binding = 0,
					.format = gfx::Format::eRg32Sfloat,
					.offset = offsetof(Vertex, texcoord0)
				}
			}
		},
		.input_assembly = {
			.primitive_topology = gfx::PrimitiveTopology::eTriangleList,
			.primitive_restart_enable = false
		},
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
			.cull_mode = gfx::CullMode::eNone
		},
		.multisample = {
			.rasterization_samples = gfx::SampleCount::e1
		},
		.depth_stencil = {
			.depth_test_enable = true,
			.depth_write_enable = true,
			.depth_bounds_test_enable = false,
			.stencil_test_enable = false,
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
	
	pipeline = driver->pipeline_create(pipeline_descriptor);
	
	camera = window_->sceneTree()->createEntity();
	editor_camera_ = &window_->sceneTree()->entity(camera)->component<EditorCamera3D>();
}

Result<> ForwardRenderer::resize(ivec2) {
	return OK;
}

Result<> ForwardRenderer::render() {
	GraphicsBackend* driver = GraphicsDriver::get();
	const RID surface = window_->surface();
	const RID command_rid = driver->begin_recording(surface);

	const RID active_image = driver->surface_get_active_image(surface);
	

	const Vec start_transition = {
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
				.aspect_mask = BitFlag(gfx::Aspect::eDepth) | BitFlag(gfx::Aspect::eStencil)
			}
		}
	};
	
	driver->transition(command_rid, start_transition);

	sceneTree()->initiateRenderSetup({
		.pass = RenderPassType::Normal,
		.material_bind_group_layout = bind_group_layout,
		.pipeline_layout = pipeline_layout,
		.pipeline = pipeline,
		.cmd = command_rid
	});

	driver->begin_rendering(surface, command_rid, pipeline, window_->depthImageView());
	
	sceneTree()->initiateDraw({
		.pass = RenderPassType::Normal,
		.material_bind_group_layout = bind_group_layout,
		.pipeline_layout = pipeline_layout,
		.pipeline = pipeline,
		.cmd = command_rid
	});
	
	driver->finish_rendering(command_rid);
	
	const Vec finish_transition = {
		ImageTransitionDescriptor{
			.image = active_image,
			.src = ImageTransitionStateDescriptor{
				.layout = gfx::ImageLayout::eAttachmentOptimal,
				.access = BitFlag(gfx::Access::eColorAttachmentWrite) | BitFlag(gfx::Access::eColorAttachmentRead),
				.stage = BitFlag(gfx::PipelineStage::eColorAttachmentOutput)
			},
			.dst = ImageTransitionStateDescriptor{
				.layout = gfx::ImageLayout::ePresent,
				.access = BitFlag(gfx::Access::eNone),
				.stage = BitFlag(gfx::PipelineStage::eColorAttachmentOutput)
			},
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = BitFlag(gfx::Aspect::eColor)
			}
		}
	};
	
	driver->transition(command_rid, finish_transition);
	
	driver->finish_recording(command_rid);

	driver->command_submit(surface, command_rid);
	driver->present(surface);
	
	return OK;
}
SharedPtr<SceneTree> ForwardRenderer::sceneTree() const {
	return window_->sceneTree();
}

RendererType ForwardRenderer::rendererType() const {
	return RendererType::FORWARD;
}

void ForwardRenderer::dispose() {
	is_disposed_ = true;
}

bool ForwardRenderer::disposed() const {
	return is_disposed_;
}
