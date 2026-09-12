#include <vulkan/vulkan.hpp>
#include "geometry_buffer.hpp"

#include "driver.hpp"
#include "window.hpp"

GBuffer::GBuffer()=default;

GBuffer::GBuffer(ivec2 const &resolution) {
	change_resolution(resolution);
}

GBuffer::~GBuffer() {
	IGpuDriver* driver = GraphicsSystem::get_driver();
	for (Storage &g_buf : storage) {
		if (g_buf.depth.valid()) driver->destroy_image(g_buf.depth);
		if (g_buf.color.valid()) driver->destroy_image(g_buf.color);
		if (g_buf.direct_light.valid()) driver->destroy_image(g_buf.direct_light);
		if (g_buf.position.valid()) driver->destroy_image(g_buf.position);
		if (g_buf.normal.valid()) driver->destroy_image(g_buf.normal);
		if (g_buf.orm.valid()) driver->destroy_image(g_buf.orm);
	}
}

void GBuffer::change_resolution(ivec2 resolution) {
	/**
	 * Memory estimations in 1080p:
	 * Double buffering by default allocates two images per buffer, this is just multiplying by 2, but if triple buffering is used then this becomes a factor of 3.
	 * 
	 * Constant: Resolution = 1920 * 1080
	 * 
	 * Depth (Level 0): 32 bits * Resolution * 2 (...math to calculate the size of all levels combined) = 33.1614 MB 
	 * Color: 8 bits * 4 channels * Resolution * 2 = 16.5888 MB
	 * Direct Light: 16 bits * 4 channels * Resolution * 2 = 33.1776 MB
	 * Normal: 16 bits * 4 channels * Resolution * 2 = 33.1776 MB
	 * Position: 16 bits * 4 channels * Resolution * 2 = 33.1776 MB
	 * ORM: 8 bits * 4 channels * Resolution * 2 = 16.5888 MB
	 * 
	 * Total: 165.8872 MB (248.8308 MB for triple buffering)
	 * 
	 */
	
	// For Hi-Z
	const u32 mip_levels = static_cast<u32>(std::floor(std::log2(std::max(resolution.x, resolution.y)))) + 1u;
	IGpuDriver* driver = GraphicsSystem::get_driver();
	for (Storage &g_buf : storage) {
		if (g_buf.depth.valid()) driver->destroy_image(g_buf.depth);
		if (g_buf.color.valid()) driver->destroy_image(g_buf.color);
		if (g_buf.direct_light.valid()) driver->destroy_image(g_buf.direct_light);
		if (g_buf.normal.valid()) driver->destroy_image(g_buf.normal);
		if (g_buf.position.valid()) driver->destroy_image(g_buf.position);
		if (g_buf.orm.valid()) driver->destroy_image(g_buf.orm);
		
		const ImageDescriptor depth_desc{
			.label = "GBuffer Depth",
			.format = gfx::Format::eDepth32Sfloat,
			.usage = gfx::ImageUsage::eDepthStencilAttachment | gfx::ImageUsage::eSampled,
			.memory_usage = gfx::MemoryUsage::ePreferDevice,
			.size = uint3(resolution, 1),
			.mip_levels = mip_levels
		};
		g_buf.depth = driver->create_image(depth_desc);

		const ImageDescriptor color_desc{
			.label = "GBuffer Color",
			.format = gfx::Format::eRgba8Srgb,
			.usage = gfx::ImageUsage::eColorAttachment | gfx::ImageUsage::eSampled,
			.memory_usage = gfx::MemoryUsage::ePreferDevice,
			.size = uint3(resolution, 1)
		};
		g_buf.color = driver->create_image(color_desc);
		
		const ImageDescriptor direct_light_desc{
			.label = "GBuffer Direct Light",
			.format = gfx::Format::eRgba16Sfloat,
			.usage = gfx::ImageUsage::eColorAttachment | gfx::ImageUsage::eSampled,
			.memory_usage = gfx::MemoryUsage::ePreferDevice,
			.size = uint3(resolution, 1)
		};
		g_buf.direct_light = driver->create_image(direct_light_desc);

		const ImageDescriptor normal_desc{
			.label = "GBuffer Normal",
			.format = gfx::Format::eRgba16Sfloat,
			.usage = gfx::ImageUsage::eColorAttachment | gfx::ImageUsage::eSampled,
			.memory_usage = gfx::MemoryUsage::ePreferDevice,
			.size = uint3(resolution, 1)
		};
		g_buf.normal = driver->create_image(normal_desc);

		const ImageDescriptor position_desc{
			.label = "GBuffer Position",
			.format = gfx::Format::eRgba16Sfloat,
			.usage = gfx::ImageUsage::eColorAttachment | gfx::ImageUsage::eSampled,
			.memory_usage = gfx::MemoryUsage::ePreferDevice,
			.size = uint3(resolution, 1)
		};
		g_buf.position = driver->create_image(position_desc);

		const ImageDescriptor orm_desc{
			.label = "GBuffer ORM",
			.format = gfx::Format::eRgba8Srgb,
			.usage = gfx::ImageUsage::eColorAttachment | gfx::ImageUsage::eSampled,
			.memory_usage = gfx::MemoryUsage::ePreferDevice,
			.size = uint3(resolution, 1)
		};
		g_buf.orm = driver->create_image(orm_desc);
	}
}

gfx::RenderingDescriptor GBuffer::get_rendering_info(const IWindow *window, const std::uint32_t frame_index) const {
	IGpuDriver* driver = GraphicsSystem::get_driver();
	const Storage& frame_storage = storage[frame_index];
	RenderingDescriptor rendering_desc{
		.color_attachments{
			RenderingAttachmentDescriptor{
				.image_view{frame_storage.color},
				.layout{gfx::ImageLayout::eColorAttachmentOptimal},
				.load_op = gfx::LoadOp::eClear,
				.store_op = gfx::StoreOp::eStore,
				.clear_color = ClearColorValue{
					.float32 = {0.0f, 0.0f, 0.0f, 1.0f}
				}
			},
			RenderingAttachmentDescriptor{
				.image_view{frame_storage.normal},
				.layout{gfx::ImageLayout::eColorAttachmentOptimal},
				.load_op = gfx::LoadOp::eClear,
				.store_op = gfx::StoreOp::eStore,
				.clear_color = ClearColorValue{
					.float32 = {0.0f, 0.0f, 0.0f, 1.0f}
				}
			},
			RenderingAttachmentDescriptor{
				.image_view{frame_storage.position},
				.layout{gfx::ImageLayout::eColorAttachmentOptimal},
				.load_op = gfx::LoadOp::eClear,
				.store_op = gfx::StoreOp::eStore,
				.clear_color = ClearColorValue{
					.float32 = {0.0f, 0.0f, 0.0f, 1.0f}
				}
			},
			RenderingAttachmentDescriptor{
				.image_view{frame_storage.orm},
				.layout{gfx::ImageLayout::eColorAttachmentOptimal},
				.load_op = gfx::LoadOp::eClear,
				.store_op = gfx::StoreOp::eStore,
				.clear_color = ClearColorValue{
					.float32 = {0.0f, 0.0f, 0.0f, 1.0f}
				}
			}
		},
		.depth_attachment = RenderingAttachmentDescriptor{
			.image_view{frame_storage.depth},
			.layout{gfx::ImageLayout::eDepthStencilAttachmentOptimal},
			.load_op = gfx::LoadOp::eClear,
			.store_op = gfx::StoreOp::eStore,
			.clear_depth_stencil = ClearDepthStencilValue{
				.depth = 1.0f,
				.stencil = 0
			}
		},
		.render_area = window->get_rect_2d()
	};
	return rendering_desc;
}

RID GBuffer::get_direct_lighting_texture(const std::uint32_t frame_index) const {
	return storage[frame_index].color;
}

RID GBuffer::get_normal_texture(const std::uint32_t frame_index) const {
	return storage[frame_index].normal;
}

RID GBuffer::get_position_texture(const std::uint32_t frame_index) const {
	return storage[frame_index].position;
}

RID GBuffer::get_orm_texture(const std::uint32_t frame_index) const {
	return storage[frame_index].orm;
}

RID GBuffer::get_depth_texture(const std::uint32_t frame_index) const {
	return storage[frame_index].depth;
}