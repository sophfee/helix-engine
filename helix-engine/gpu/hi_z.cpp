#include "hi_z.hpp"

#include "driver.hpp"
#include "spirv.hpp"

HiZ::HiZ() : hi_z_shader_(spirv::load("shaders/vulkan/generate_hi_z.spv")) {
	IGpuDriver* driver = GraphicsSystem::get_driver();
	
	Vector entries = {
		gfx::sampled_image_binding(0, gfx::ImageLayout::eReadOnly),
		gfx::image_binding(2, gfx::ImageLayout::eGeneral),
		gfx::sampler_binding(2)
	};

	const BindGroupLayoutDescriptor bind_group_layout_descriptor{
		.label = "Hi-Z Bind Group Layout",
		.entries = {
			{
				.binding = 0,
				.visibility = gfx::ShaderStage::eCompute,
				.type = gfx::BindingType::eSampledImage
			},
			{
				.binding = 1,
				.visibility = gfx::ShaderStage::eCompute,
				.type = gfx::BindingType::eStorageImage
			},
			{
				.binding = 2,
				.visibility = gfx::ShaderStage::eCompute,
				.type = gfx::BindingType::eSampler
			}
		}
	};
	
	hi_z_bind_group_layout_ = driver->create_bind_group_layout(bind_group_layout_descriptor);
	
	const PipelineLayoutDescriptor pipeline_layout_descriptor{
		.label = "Hi-Z Pipeline Layout",
		.bind_group_layouts = { hi_z_bind_group_layout_ }
	};
	hi_z_pipeline_layout_ = driver->create_pipeline_layout(pipeline_layout_descriptor);
	
	const gfx::ComputePipelineDescriptor compute_pipeline_descriptor{
		.label = "Hi-Z Pipeline",
		.layout = hi_z_pipeline_layout_,
		.stage = {
			.shader = hi_z_shader_,
			.stage = gfx::ShaderStage::eCompute,
			.entry_point = "main"
		}
	};
	
	hi_z_pipeline_ = driver->create_pipeline(compute_pipeline_descriptor);
}

HiZ::~HiZ() {
	IGpuDriver* driver = GraphicsSystem::get_driver();
	driver->destroy_pipeline(hi_z_pipeline_);
	driver->destroy_pipeline_layout(hi_z_pipeline_layout_);
	driver->destroy_bind_group_layout(hi_z_bind_group_layout_);
	driver->destroy_shader(hi_z_shader_);
}
