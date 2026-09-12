#include "contact_shadows.hpp"

#include "spirv.hpp"

ContactShadows::ContactShadows() : shader_(spirv::load("shaders/vulkan/contact_shadows.spv")) {
	IGpuDriver *driver = GraphicsSystem::get_driver();

	const BindGroupLayoutDescriptor bind_group_layout_descriptor{
		.label = "Contact Shadows Bind Group Layout",
		.entries = {
			gfx::sampled_image_binding(0, gfx::ShaderStage::eCompute),
			gfx::image_binding(1, gfx::ShaderStage::eCompute),
			gfx::sampler_binding(2, gfx::ShaderStage::eCompute)
		}
	};
	bind_group_layout_ = driver->create_bind_group_layout(bind_group_layout_descriptor);

	const PipelineLayoutDescriptor pipeline_layout_descriptor{
		.label = "Contact Shadows Pipeline Layout",
		.bind_group_layouts = {bind_group_layout_}
	};
	pipeline_layout_ = driver->create_pipeline_layout(pipeline_layout_descriptor);

	const gfx::ComputePipelineDescriptor compute_pipeline_descriptor{
		.label = "Contact Shadows Pipeline",
		.layout = pipeline_layout_,
		.stage = gfx::pipeline_stage(shader_, gfx::ShaderStage::eCompute)
	};
	pipeline_ = driver->create_pipeline(compute_pipeline_descriptor);
}

ContactShadows::~ContactShadows() {
	IGpuDriver *driver = GraphicsSystem::get_driver();
	driver->destroy_pipeline(pipeline_);
	driver->destroy_pipeline_layout(pipeline_layout_);
	driver->destroy_bind_group_layout(bind_group_layout_);
	driver->destroy_shader(shader_);
}

void ContactShadows::record(IRenderer* renderer, const RID command, Optional<RID> surface) {
	IGpuDriver *driver = GraphicsSystem::get_driver();
	
	driver->bind_pipeline(pipeline_, command, gfx::PipelineBindPoint::eCompute);
	
	
}