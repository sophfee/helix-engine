#pragma once

#include "types.hpp"
#include "math.hpp"
#include "texture.h"
#include "buffer.h"

//class Texture;
//class Buffer;

//< Glsl Types
struct PointLight {
	vec3 Position;
	float Range;
    
	vec3 Color; //< HDR color, not clamped to [0, 1]. Intensity should essentially be multiplied into the color.
	int ShadowMapIndex; //< -1 if no shadow map, otherwise index into array of shadow maps
};

struct SpotLight {
	vec3 Position;
	float Range;
    
	vec3 Direction;
	float InnerConeCos;
    
	vec3 Color; //< HDR color, not clamped to [0, 1]. Intensity should essentially be multiplied into the color.
	float OuterConeCos;
    
	int ShadowMapIndex; //< -1 if no shadow map, otherwise index into array of shadow maps
};

struct alignas(16) PointShadow {
	u64 ShadowTexture;
	int ShadowTextureIndex;
	int _pad0;
	mat4 LightViewProj[6];
    
	vec3 Position;
	int LightIndex; //< Index into array of point lights, this allows the two to be associated without needing to duplicate the light data in the shadow struct
    
	float NearPlane;
	float FarPlane;
};

struct SpotShadow {
	u64 ShadowTexture;
	u64 _pad0;
	mat4 LightViewProj;

	vec3 Position;
	int LightIndex;

	float NearPlane;
	float FarPlane;
};

class LightingSystem {

	static constexpr auto MAX_POINT_SHADOWS = 64;
	static constexpr auto MAX_SPOT_SHADOWS = 64;

	static constexpr auto POINT_LIGHT_BUFFER_BINDING = 10;
	static constexpr auto SPOT_LIGHT_BUFFER_BINDING = 11;
	static constexpr auto POINT_SHADOW_BUFFER_BINDING = 12;
	static constexpr auto SPOT_SHADOW_BUFFER_BINDING = 13;

	Vec<Box<Framebuffer>> pointShadowFramebuffers;
	Vec<Box<Texture>> pointShadowTextures;
	Stack<int> pointShadowStack; //< Used to determine how to give out textures
	
	Vec<Box<Texture>>  spotShadowTextures;
	Stack<int> spotShadowStack;

	Box<Buffer> pointLightBuffer;
	PointLight *pointLightBufferData = nullptr;
	
	Stack<int> pointLightStack;
	int pointLightCount = 0;
	
	Box<Buffer> spotLightBuffer;
	SpotLight  *spotLightBufferData = nullptr;
	
	Stack<int> spotLightStack;
	int spotLightCount = 0;
	
	Box<Buffer> pointShadowBuffer;
	PointShadow *pointShadowBufferData = nullptr;
	
	Box<Buffer> spotShadowBuffer;
	SpotShadow *spotShadowBufferData = nullptr;

	Box<Program> pointShadowProgram_;
	
	LightingSystem();

public:
	static LightingSystem *singleton();

	Program &pointShadowProgram();

	void startWritingPointShadows();
	void stopWritingPointShadows();

	[[nodiscard]] std::optional<int> checkOutPointShadow();
	void checkInPointShadow(int index);
	
	[[nodiscard]] Texture const &pointShadowTexture(int index) const;
	[[nodiscard]] Framebuffer const &pointShadowFramebuffer(int index) const;
	void setPointShadow(int index, PointShadow const &shadow);

	void startWritingPointLights();
	void stopWritingPointLights();

	[[nodiscard]] std::optional<int> checkOutPointLight();
	void checkInPointLight(int index);
	
	[[nodiscard]] std::optional<int> checkOutSpotShadow();
	void checkInSpotShadow(int index);

	void setPointLight(int index, PointLight const &light);

	void prerender();
};
