#include "imgui.h"
#include "light.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "transform.h"
#include "gpu/lighting.hpp"
#include "gpu/spirv.hpp"
#include "gpu/renderers/renderer.hpp"

ComponentProvider<OmniLight> ComponentProvider<OmniLight>::instance_ = ComponentProvider();

void OmniLight::update_point_light() const {
	if (!enabled_) return;
	Transform const &xform = get_entity()->get_component<Transform>();
	intensity_ = intensity_ == 0.00f ? 16.0f : intensity_;
	PointLight const light{
		.position = xform.get_position(),
		.range = range_,
		.color = color_ * intensity_//,
		//.shadow_map_index = shadows_enabled_ ? static_cast<int>(shadow_index_) : -1
	};
	LightingSystem::singleton()->set_point_light(light_index_, light);
	dirty_ = true;
}

void OmniLight::update_point_shadow() const {
	if (!shadows_enabled_ || shadow_index_ == -1)
		return;
	
	dirty_ = true; //< mark for re-render
	Transform const &xform = get_entity()->get_component<Transform>();

	//< Perspective is consistent
	mat4 const proj = glm::perspective(
		90.0f,
		1.0f,
		near_,
		far_
	);
	
	//< Generate 6 directions for the cubemap shadow map
	PointShadow shadow{};

	RID texture = LightingSystem::singleton()->get_point_shadow_image(shadow_index_);
	
	auto const lightProj = (mat4*)&shadow.LightViewProj;
	lightProj[0] = proj * glm::lookAt(
		               xform.get_position(),
		               xform.get_position() + vec3(+1.0, +0.0, +0.0),
		               vec3(0.0, -1.0, 0.0)
	               );
	// std::cout << "lightProj[0]\n";
	// print_matrix(lightProj[0]);
	lightProj[1] = proj * glm::lookAt(
		               xform.get_position(),
		               xform.get_position() + vec3(-1.0, +0.0, +0.0),
		               vec3(0.0, -1.0, 0.0)
	               );
	lightProj[2] = proj * glm::lookAt(
		               xform.get_position(),
		               xform.get_position() + vec3(+0.0, +1.0, +0.0),
		               vec3(0.0, 0.0, 1.0)
	               );
	lightProj[3] = proj * glm::lookAt(
		               xform.get_position(),
		               xform.get_position() + vec3(+0.0, -1.0, +0.0),
		               vec3(0.0, 0.0, -1.0)
	               );
	lightProj[4] = proj * glm::lookAt(
		               xform.get_position(),
		               xform.get_position() + vec3(+0.0, +0.0, +1.0),
		               vec3(0.0, -1.0, 0.0)
	               );
	lightProj[5] = proj * glm::lookAt(
		               xform.get_position(),
		               xform.get_position() + vec3(+0.0, +0.0, -1.0),
		               vec3(0.0, -1.0, 0.0)
	               );

	shadow.Position = xform.get_position();
	shadow.LightIndex = static_cast<int>(light_index_);

	shadow.NearPlane = near_;
	shadow.FarPlane  = far_;

	LightingSystem::singleton()->set_point_shadow(shadow_index_, shadow);
}


OmniLight::OmniLight() : Component(), data_({}) {
}

OmniLight::OmniLight(Weak<SceneTree> const &scene_tree, const RID ent) : Component(scene_tree, ent), data_({}) {
}

OmniLight::~OmniLight() = default;

bool OmniLight::dirty() const {
	return dirty_;
}

vec3 OmniLight::get_position() const {
	Transform const &xform = get_entity()->get_component<Transform>();
	return xform.get_position();
} 

vec3 OmniLight::get_color() const {
	return color_;
}

float OmniLight::get_range() const {
	return range_;
}

float OmniLight::get_intensity() const {
	return intensity_;
}

void OmniLight::set_position(vec3 const &value) {
	Transform &xform = get_entity()->get_component<Transform>();
	xform.translation = value;
	update_point_light();
	update_point_shadow();
}

void OmniLight::set_color(vec3 const &value) {
	color_ = value;
	update_point_light();
	update_point_shadow();
}

void OmniLight::set_range(float const value) {
	range_ = value;
	far_ = value;
	update_point_light();
	update_point_shadow();
}

void OmniLight::set_intensity(float const value) {
	intensity_ = value;
	update_point_light();
	update_point_shadow();
}

void OmniLight::set_enabled(bool const enabled) {
	if (enabled == enabled_) //< State change?
		return;
	
	if (enabled) {
		auto const opt_idx = LightingSystem::singleton()->check_out_point_light();
		if (opt_idx.has_value()) {
			light_index_ = opt_idx.value();
			update_point_light();
			update_point_shadow();
			enabled_ = true;
		}
		else {
			light_index_ = -1;
			shadow_index_ = -1;
			enabled_ = false;
		}
	}
	else {
		LightingSystem::singleton()->check_in_point_light(light_index_);
		light_index_ = -1;
		shadow_index_ = -1;
		enabled_ = false;
	}
}

bool OmniLight::is_enabled() const {
	return enabled_;
}

void OmniLight::set_shadows_enabled(bool const enabled) {
	if (enabled == shadows_enabled_)
		return;

	if (enabled) {
		auto const opt_idx = LightingSystem::singleton()->check_out_point_shadow();
		if (opt_idx.has_value()) {
			shadow_index_ = opt_idx.value();
			shadows_enabled_ = true;
			update_point_light();
			update_point_shadow();
		}
	}
	else {
		RID texture = LightingSystem::singleton()->get_point_shadow_image(shadow_index_);
		LightingSystem::singleton()->check_in_point_shadow(shadow_index_);
		update_point_light();
		update_point_shadow();
		shadows_enabled_ = false;
		shadow_index_ = -1;
	}
}

bool OmniLight::get_shadows_enabled() const {
	return shadows_enabled_;
}
void OmniLight::editor() {
	using namespace ImGui;
	
	TableHeader("Omnidirectional Light");
	TableNextColumn();
	TableHeader("##pointlight");
	TableNextRow();
	TableNextColumn();
	
	Text("Enabled");
	TableNextColumn();

	if (Checkbox("##enabled", &enabled_)) {
		if (enabled_) {
			const auto opt_idx = LightingSystem::singleton()->check_out_point_light();
			if (opt_idx.has_value()) {
				light_index_ = opt_idx.value();
				update_point_light();
				update_point_shadow();
				enabled_ = true;
			}
			else {
				light_index_ = -1;
				shadow_index_ = -1;
				enabled_ = false;
			}
		}
		else {
			LightingSystem::singleton()->check_in_point_light(light_index_);
			light_index_ = -1;
			shadow_index_ = -1;
			enabled_ = false;
		}
	}
	TableNextRow();
	TableNextColumn();

	if (is_enabled()) {
		Text("Enable shadows?");
		TableNextColumn();
		if (Checkbox("##shadows_enabled", &shadows_enabled_)) {
			if (shadows_enabled_) {
				const auto opt_idx = LightingSystem::singleton()->check_out_point_shadow();
				if (opt_idx.has_value()) {
					shadow_index_ = opt_idx.value();
					update_point_light();
					update_point_shadow();
				}
				else {
					throw std::exception("OmniLight: No available point shadow slots to enable shadows for this light. Consider increasing the maximum point shadow count in LightingSystem.");
				}
			}
			else {
				RID texture = LightingSystem::singleton()->get_point_shadow_image(shadow_index_);
				LightingSystem::singleton()->check_in_point_shadow(shadow_index_);
				shadow_index_ = -1;
				update_point_light();
				update_point_shadow();
			}
		}
		TableNextRow();
		TableNextColumn();
		Text("Color");
		TableNextColumn();
		SetNextItemWidth(-FLT_MIN);
		if (ColorEdit3("##Color", &color_[0])) { update_point_light(); }
		TableNextRow();
		TableNextColumn();
		Text("Intensity");
		TableNextColumn();
		SetNextItemWidth(-FLT_MIN);
		if (SliderFloat("##Intensity", &intensity_, 0.0f, 64.0f)) { update_point_light(); }
		TableNextRow();
		TableNextColumn();
		Text("Range");
		TableNextColumn();
		SetNextItemWidth(-FLT_MIN);
		if (SliderFloat("##Range", &range_, 0.0f, 64.0f)) { update_point_light(); }
		if (get_shadows_enabled()) {
			TableNextRow();
			TableNextColumn();
			Text("Near Z");
			TableNextColumn();
		SetNextItemWidth(-FLT_MIN);
			if (SliderFloat("##near", &near_, 0.01f, far_ - 0.01f)) { update_point_shadow(); }
			TableNextRow();
			TableNextColumn();
			Text("Far Z");
			TableNextColumn();
		SetNextItemWidth(-FLT_MIN);
			if (SliderFloat("##far", &far_, near_ + 0.01f, 128.0f)) { update_point_shadow(); }
		}
	}
}
void OmniLight::update(double x) {
	static RenderPassInfo ri{
		.pass = RenderPassType::Shadow,
	};

	//< Setting up for point shadow pass.
	if (!enabled_ || !shadows_enabled_ || !dirty_) return; //< 
	dirty_ = false;                                        //< Don't re-render shadow depth if not needed.
}

OmniLightShadowPass::OmniLightShadowPass() : shader_(spirv::load("shaders/vulkan/depth_pass.spv")) {
	
	using enum gfx::ShaderStage;
	using enum gfx::Format;
	using enum gfx::PrimitiveTopology;
	using enum gfx::CompareOp;
	using enum gfx::CullMode;
	using enum gfx::FrontFace;
	using enum gfx::PolygonMode;
	
	IGpuDriver *driver = GraphicsSystem::get_driver();
	
	bind_group_layout_ = driver->create_bind_group_layout(gfx::empty<BindGroupLayoutDescriptor>("OmniLight Shadow Pass Bind Group Layout"));
	
	const PipelineLayoutDescriptor pipeline_layout_descriptor{
		.label = "OmniLight Shadow Pass Pipeline Layout",
		.bind_group_layouts = {bind_group_layout_},
		.push_constants = {gfx::push_constant<GpuDeviceAddress, 2>(eFragment | eVertex)}
	};
	pipeline_layout_ = driver->create_pipeline_layout(pipeline_layout_descriptor);

	const GraphicsPipelineDescriptor pipeline_descriptor{
		.label = "OmniLight Shadow Pass Pipeline",
		.layout = pipeline_layout_,
		.stages = {
			gfx::pipeline_stage(shader_, eVertex),
			gfx::pipeline_stage(shader_, eFragment)
		},
		.rendering = {
			.color_formats = {},
			.depth_format = eDepth16Unorm,
			.stencil_format = eUndefined
		},
		.vertex_input = Vertex::input_state(),
		.input_assembly = gfx::input_assembly(eTriangleList),
		.viewport = {
			.viewports = {
				Viewport{
					.x = 0.0f,
					.y = 0.0f,
					.width = 1024.0f,
					.height = 1024.0f,
					.min_depth = 0.0f,
					.max_depth = 1.0f
				}
			},
			.scissors = {
				Rect2D{
					.offset = Offset2D{.x = 0, .y = 0 },
					.extent = Extent2D{.width = 1024, .height = 1024 }
				}
			}
		},
		.rasterization = {},
		.multisample = {},
		.depth_stencil = DepthStencilDescriptor::enabled()
	};
	pipeline_ = driver->create_pipeline(pipeline_descriptor);
}

OmniLightShadowPass::~OmniLightShadowPass() {
	IGpuDriver* driver = GraphicsSystem::get_driver();
	driver->destroy_pipeline(pipeline_);
	driver->destroy_pipeline_layout(pipeline_layout_);
	driver->destroy_bind_group_layout(bind_group_layout_);
}

void OmniLightShadowPass::record(IRenderer *renderer, RID command, Optional<RID> surface, OmniLight *light) {
	LightingSystem* lighting = LightingSystem::singleton();
	IGpuDriver* driver = GraphicsSystem::get_driver();
	if (light->get_shadows_enabled() && light->dirty()) {
		RID image_view = lighting->get_point_shadow_image_view(light->shadow_index_);
		
		driver->begin_rendering(command, {
			.color_attachments = {},
			.depth_attachment = RenderingAttachmentDescriptor{
				.image_view = image_view,
				.load_op = gfx::LoadOp::eClear,
				.store_op = gfx::StoreOp::eStore,
				.clear_depth_stencil = ClearDepthStencilValue{ 1.0f, 0 }
			},
			.render_area = Rect2D::from_size(1024u, 1024u)
		});
		
		driver->bind_pipeline(pipeline_, command, gfx::PipelineBindPoint::eGraphics);

		const Transform& transform = light->get_entity()->get_component<Transform>();

		const SharedPtr<const SceneTree> tree_ptr = light->get_scene_tree();
		SceneTree* tree = const_cast<SceneTree*>(tree_ptr.get());
		
		tree->init_draw(RenderPassInfo{
			.pass = RenderPassType::Shadow,
			.view = transform.get_matrix(),
			.projection = glm::perspective(glm::radians(90.0f), 1.0f, light->near_, light->far_),
			.pipeline_layout = pipeline_layout_,
			.pipeline = pipeline_,
			.cmd = command,
			.frame_index = 0
		});
		
		driver->finish_rendering(command, false);
	}
}
