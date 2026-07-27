

#include "lighting.hpp"

#include <iostream>

#include "driver.hpp"

LightingSystem::LightingSystem()
	: pointShadowImages(0),
	  spotShadowImages(0) {
}

LightingSystem * LightingSystem::singleton() {
	static LightingSystem singleton;
	return &singleton;
}

RID LightingSystem::pointShadowProgram() {
	return pointShadowProgram_;
}

void LightingSystem::startWritingPointShadows() {
	if (pointShadowBufferData != nullptr)
		return; // Already mapped for writing
}

void LightingSystem::stopWritingPointShadows() {
	if (pointShadowBufferData == nullptr)
		return;
}

std::optional<int> LightingSystem::checkOutPointShadow() {
	if (pointShadowStack.empty())
		return std::nullopt;
	
	int const index = pointShadowStack.top();
	pointShadowStack.pop();
	return index;
}

void LightingSystem::checkInPointShadow(int const index) {
	pointShadowStack.push(index);
}

RID LightingSystem::pointShadowTexture(int const index) const {
	return pointShadowImages[index]; // supports_bindless_textures() ? *pointShadowImages[index] : *pointShadowImages.back();
}

void LightingSystem::setPointShadow(int const index, PointShadow const &shadow) {
#ifndef _DEBUG
	assert(pointShadowBufferData != nullptr && "Point shadow buffer is not mapped for writing.");
#else
	if (pointShadowBufferData == nullptr)
		startWritingPointShadows();
#endif

#ifdef _DEBUG
	
	// std::cout << "Setting point shadow at index " << index << ":\n";
	// std::cout << "  Shadow Texture Handle: " << shadow.ShadowTexture << "\n";
	// std::cout << "  Position: " << shadow.Position.x << " " << shadow.Position.y << " " << shadow.Position.z << "\n";
	// std::cout << "  Light Index: " << shadow.LightIndex << "\n";
	// std::cout << "  Near Plane: " << shadow.NearPlane << "\n";
	// std::cout << "  Far Plane: " << shadow.FarPlane << "\n";
	

	// printf("LightViewProj[0][0][0] = %f\n", shadow.LightViewProj[0][0][0]);
	// printf("Position = %f %f %f\n", shadow.Position.x, shadow.Position.y, shadow.Position.z);
	
#endif
	pointShadowBufferData[index] = shadow;
}

void LightingSystem::startWritingPointLights() {
	if (pointLightBufferData != nullptr)
		return; // Already mapped for writing
}

void LightingSystem::stopWritingPointLights() {
	if (pointLightBufferData == nullptr)
		return;
}

std::optional<int> LightingSystem::checkOutPointLight() {
	if (pointLightStack.empty())
		return std::nullopt;

	int const index = pointLightStack.top();
	pointLightStack.pop();
	return index;
}

void LightingSystem::checkInPointLight(int const index) {
	pointLightStack.push(index);
}

std::optional<int> LightingSystem::checkOutSpotShadow() {
	if (spotShadowStack.empty())
		return std::nullopt;
	
	int const index = spotShadowStack.top();
	spotShadowStack.pop();
	return index;
}

void LightingSystem::checkInSpotShadow(int const index) {
	spotShadowStack.push(index);
}

void LightingSystem::setPointLight(int index, PointLight const &light)  {
#ifdef STRICT
	assert(pointLightBufferData != nullptr && "Point light buffer is not mapped for writing.");
#else
	startWritingPointLights();
#endif
	pointLightBufferData[index] = light;
}

void LightingSystem::prerender() {
	stopWritingPointShadows(); //< Prevent a possible mapped buffer being bound
	stopWritingPointLights();  //< Prevent a possible mapped buffer being bound
}

void LightingSystem::dispose() {
	stopWritingPointLights();
	stopWritingPointShadows();
	GraphicsDriver* driver = GraphicsDriver::singleton();
	
	for (const RID pointShadowImageView : pointShadowImageViews)
		driver->image_view_delete(pointShadowImageView);
	
	for (const RID spotShadowImageView : spotShadowImageViews)
		driver->image_view_delete(spotShadowImageView);
	
	for (const RID pointShadowImage : pointShadowImages)
		driver->image_delete(pointShadowImage);
	
	for (const RID pointShadowImage : pointShadowImages)
		driver->image_delete(pointShadowImage);
	
	pointLightBufferData = nullptr;
	spotLightBufferData = nullptr;
	pointShadowBufferData = nullptr;
	spotShadowBufferData = nullptr;
	
	driver->buffer_delete(pointLightBuffer);
	driver->buffer_delete(pointShadowBuffer);
	driver->buffer_delete(spotLightBuffer);
	driver->buffer_delete(spotShadowBuffer);
	
	disposed_ = true;
}

bool LightingSystem::disposed() const {
	return disposed_;
}
