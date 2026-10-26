#include "image.hpp"

#include "driver.hpp"

using namespace gfx;

Image::Image() : image_(GraphicsSystem::get_driver()->create_image()) {
}

Image::~Image() {
	Image::free();
}

void Image::free() {
	GraphicsSystem::get_driver()->destroy_image(image_);
}

/// NOT YET IMPLEMENTED
void Image::queue_free() {
	GraphicsSystem::get_driver()->destroy_image(image_);
}

bool Image::is_valid() const {
	return image_.valid();
}

RID Image::get_subresource(int subresource) const {
	if (subresource == 0) return image_;
	return GraphicsSystem::get_driver()->get_associated_image_view(image_, subresource - 1);
}

int Image::get_subresource_count() const {
	Vector<RID> _;
	return static_cast<int>(GraphicsSystem::get_driver()->get_associated_image_views(image_, _));
}
