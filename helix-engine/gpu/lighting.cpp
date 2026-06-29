

#include "lighting.hpp"

#include <iostream>

#include "framebuffer.h"
#include "render_server.h"

namespace {
	TextureCubeMapBuilder pointShadowCubeMapBuilder() {
		return TextureCubeMapBuilder()
			.internalFormat(gl::InternalFormat::DepthComponent16)
			.pixelFormat(gl::PixelFormat::DepthComponent)
			.pixelType(gl::PixelType::HalfFloat)
			.compareMode(gl::TextureCompareMode::CompareRefToTexture)
			.compareFunc(gl::CompareFunction::Less)
			.resolution(ivec2(2048, 2048));
	}

	TextureCubeMapArrayBuilder pointShadowCubeMapArrayBuilder() {
		return TextureCubeMapArrayBuilder()
			.internalFormat(gl::InternalFormat::DepthComponent16)
			.pixelFormat(gl::PixelFormat::DepthComponent)
			.pixelType(gl::PixelType::HalfFloat)
			.compareMode(gl::TextureCompareMode::CompareRefToTexture)
			.compareFunc(gl::CompareFunction::Less)
			.resolution(ivec3(1024, 1024, LightingSystem::MAX_POINT_SHADOWS * 6));
	}

	Texture2DBuilder spotShadowMapBuilder() {
		return Texture2DBuilder()
			.internalFormat(gl::InternalFormat::DepthComponent16)
			.pixelFormat(gl::PixelFormat::DepthComponent)
			.pixelType(gl::PixelType::HalfFloat)
			.compareMode(gl::TextureCompareMode::CompareRefToTexture)
			.compareFunc(gl::CompareFunction::Less)
			.resolution(ivec2(2048, 2048));
	}
	

	bool supports_bindless_textures() {
		return RenderServer::singleton().extensionSupported("GL_ARB_bindless_texture");
	}
}

LightingSystem::LightingSystem()
	: pointShadowTextures(0),
	  spotShadowTextures(0) {

	using enum gl::FramebufferAttachment;
	using enum gl::FramebufferStatus;
	if (true) {
		for (int i = 0; i < MAX_POINT_SHADOWS; ++i) {
			auto fb = std::make_unique<Framebuffer>();
			auto tx = std::make_unique<Texture>(pointShadowCubeMapBuilder());
			Texture const &texture = *tx;
			fb->attachTexture(DepthAttachment, texture);
			fb->setDrawBuffers({});
			fb->setReadBuffer(std::nullopt);
			assert(fb->status() == FramebufferComplete);
			pointShadowTextures.push_back(std::move(tx));
			pointShadowFramebuffers.push_back(std::move(fb));
			assert(pointShadowTextures.size() == pointShadowFramebuffers.size());
		}
	}
	else {
		pointShadowTextures.push_back(std::make_unique<Texture>(pointShadowCubeMapArrayBuilder()));
		auto fb = std::make_unique<Framebuffer>();
		Texture const &texture = *pointShadowTextures.back();
		fb->attachTexture(DepthAttachment, texture);
		fb->setDrawBuffers({});
		fb->setReadBuffer(std::nullopt);
		assert(fb->status() == FramebufferComplete);
	}

	for (int i = 0; i < MAX_SPOT_SHADOWS; ++i) {
		spotShadowTextures.push_back(std::make_unique<Texture>(spotShadowMapBuilder()));
	}
	
	using enum gl::BufferStorageMask;

	pointLightBuffer = std::make_unique<TypedBuffer<PointLight>>();
	pointLightBuffer->allocateElements(
		MAX_POINT_SHADOWS, nullptr,
		MapWriteBit | MapPersistentBit | ClientStorageBit
	);
	
	spotLightBuffer  = std::make_unique<TypedBuffer<SpotLight>>();

	pointShadowBuffer = std::make_unique<TypedBuffer<PointShadow>>();
	pointShadowBuffer->allocateElements(
		MAX_POINT_SHADOWS, nullptr,
		MapWriteBit | MapPersistentBit | ClientStorageBit
	);
	
	spotShadowBuffer = std::make_unique<TypedBuffer<SpotShadow>>();
	spotShadowBuffer->allocateElements(
		MAX_SPOT_SHADOWS, nullptr,
		MapWriteBit | MapPersistentBit | ClientStorageBit
	);

	for (int i = 0; i < MAX_POINT_SHADOWS; ++i) {
		pointShadowStack.push(MAX_POINT_SHADOWS - i - 1);
		pointLightStack.push(MAX_POINT_SHADOWS - i - 1);
	}
	
	for (int i = 0; i < MAX_SPOT_SHADOWS; ++i) {
		spotShadowStack.push(MAX_SPOT_SHADOWS - i - 1);
		// spotlight stuff will be implemented later on.
	}
}

LightingSystem * LightingSystem::singleton() {
	static LightingSystem singleton;
	return &singleton;
}

Program &LightingSystem::pointShadowProgram() {
	if (!pointShadowProgram_) {
		pointShadowProgram_ = std::make_unique<Program>(
			"shaders\\point_shadows.vert",
			"shaders\\point_shadows.geom",
			"shaders\\point_shadows.frag"
		);
	}
	return *pointShadowProgram_;
}

void LightingSystem::startWritingPointShadows() {
	if (pointShadowBufferData != nullptr)
		return; // Already mapped for writing
	
	using enum gl::MapBufferAccessMask;
	pointShadowBufferData = pointShadowBuffer->mapElementsRange(
		0, MAX_POINT_SHADOWS,
		MapWriteBit | MapPersistentBit | MapFlushExplicitBit
	);
}

void LightingSystem::stopWritingPointShadows() {
	if (pointShadowBufferData == nullptr)
		return;
	
	assert(pointShadowBuffer->unmap());
	pointShadowBufferData = nullptr;
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

Texture const & LightingSystem::pointShadowTexture(int const index) const {
	return *pointShadowTextures[index]; // supports_bindless_textures() ? *pointShadowTextures[index] : *pointShadowTextures.back();
}

Framebuffer const & LightingSystem::pointShadowFramebuffer(int const index) const {
	return *pointShadowFramebuffers[index]; // supports_bindless_textures() ? *pointShadowFramebuffers[index] : *pointShadowFramebuffers.back();
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
	(*pointShadowBuffer)[index] = shadow;
	pointShadowBuffer->flushMappedElementsRange(index, 1);
}

void LightingSystem::startWritingPointLights() {
	if (pointLightBufferData != nullptr)
		return; // Already mapped for writing
	
	using enum gl::MapBufferAccessMask;
	pointLightBufferData = pointLightBuffer->mapElementsRange(
		0, MAX_POINT_SHADOWS,
		MapWriteBit | MapPersistentBit | MapFlushExplicitBit
	);
}

void LightingSystem::stopWritingPointLights() {
	if (pointLightBufferData == nullptr)
		return;

	pointLightBuffer->flushMappedElementsRange(
		0, MAX_POINT_SHADOWS
	);
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
	(*pointLightBuffer)[index] = light;
	pointLightBuffer->flushMappedElementsRange(index, 1);
}

void LightingSystem::prerender() {
	stopWritingPointShadows(); //< Prevent a possible mapped buffer being bound
	stopWritingPointLights();  //< Prevent a possible mapped buffer being bound
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	using enum gl::BufferTargetARB;
	pointLightBuffer->bindBufferBase(ShaderStorageBuffer, POINT_LIGHT_BUFFER_BINDING);
	pointShadowBuffer->bindBufferBase(ShaderStorageBuffer, POINT_SHADOW_BUFFER_BINDING);
}

void LightingSystem::dispose() {
	stopWritingPointLights();
	stopWritingPointShadows();
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	pointShadowFramebuffers.clear();
	pointShadowTextures.clear();
	spotShadowTextures.clear();
	
	pointLightBufferData = nullptr;
	pointLightBuffer.reset();
	pointLightBuffer = nullptr;

	spotLightBufferData = nullptr;
	spotLightBuffer.reset();
	spotLightBuffer = nullptr;

	pointShadowBufferData = nullptr;
	pointShadowBuffer.reset();
	pointShadowBuffer = nullptr;

	spotShadowBufferData = nullptr;
	spotShadowBuffer.reset();
	spotShadowBuffer = nullptr;
	
	pointShadowProgram_.reset();

	disposed_ = true;
}

bool LightingSystem::disposed() const {
	return disposed_;
}
