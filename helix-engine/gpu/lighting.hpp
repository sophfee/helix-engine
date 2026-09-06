#pragma once

#include "types.hpp"
#include "math.hpp"
#include "engine/disposable.hpp"
#include "engine/rid.hpp"

//class Texture;
//class Buffer;

//< Glsl Types
struct PointLight {
	float3 position;
	float range;
    
	float3 color; //< HDR color, not clamped to [0, 1]. Intensity should essentially be multiplied into the color.
	int shadow_map_index; //< -1 if no shadow map, otherwise index into array of shadow maps
};

struct SpotLight {
	float3 position;
	float range;
    
	float3 direction;
	float inner_cone_cos;
    
	float3 color; //< HDR color, not clamped to [0, 1]. Intensity should essentially be multiplied into the color.
	float outer_cone_cos;
    
	//int ShadowMapIndex; //< -1 if no shadow map, otherwise index into array of shadow maps
};

struct alignas(16) PointShadow {
	u64 ShadowTexture;
	int ShadowTextureIndex;
	int _pad0;
	float4x4 LightViewProj[6];
    
	float3 Position;
	int LightIndex; //< Index into array of point lights, this allows the two to be associated without needing to duplicate the light data in the shadow struct
    
	float NearPlane;
	float FarPlane;
};

struct SpotShadow {
	u64 ShadowTexture;
	u64 _pad0;
	float4x4 LightViewProj;

	float3 Position;
	int LightIndex;

	float NearPlane;
	float FarPlane;
};

class LightingSystem : public IDisposable {
public:
	static constexpr std::size_t MAX_POINT_LIGHTS = 1024;
	static constexpr std::size_t MAX_POINT_LIGHTS_IN_BYTES = sizeof(PointLight) * MAX_POINT_LIGHTS;
	static constexpr std::size_t MAX_POINT_SHADOWS = 64;
	static constexpr std::uint32_t POINT_SHADOW_RESOLUTION = 1024u;
	static constexpr std::size_t MAX_SPOT_LIGHTS = 1024;
	static constexpr std::size_t MAX_SPOT_LIGHTS_IN_BYTES = sizeof(SpotLight) * MAX_SPOT_LIGHTS;
	
	static constexpr auto MAX_SPOT_SHADOWS = 4;

	static constexpr auto POINT_LIGHT_BUFFER_BINDING = 10;
	static constexpr auto SPOT_LIGHT_BUFFER_BINDING = 11;
	static constexpr auto POINT_SHADOW_BUFFER_BINDING = 12;
	static constexpr auto SPOT_SHADOW_BUFFER_BINDING = 13;

public:
	Vector<RID> point_shadow_images_;
	Vector<RID> point_shadow_image_views_;
	Stack<int> point_shadow_stack_; //< Used to determine how to give out textures
	
	Vector<RID> spot_shadow_images_;
	Vector<RID> spot_shadow_image_views_;
	Stack<int> spot_shadow_stack_;

	RID point_light_buffer_;
	PointLight *point_light_buffer_data_ = nullptr;
	
	Stack<int> point_light_stack_;
	int point_light_count = 0;
	
	RID spot_light_buffer_;
	SpotLight  *spot_light_buffer_data_ = nullptr;
	
	Stack<int> spot_light_stack_;
	int spot_light_count_ = 0;
	
	RID point_shadow_buffer_;
	PointShadow *point_shadow_buffer_data_ = nullptr;
	
	RID spot_shadow_buffer_;
	SpotShadow *spot_shadow_buffer_data_ = nullptr;

	RID pointShadowProgram_;

	bool disposed_ = false;
	
	LightingSystem();

public:
	static LightingSystem *singleton();

	RID get_point_shadow_program();

	void start_writing_point_shadows();
	void stop_writing_point_shadows();

	[[nodiscard]] std::optional<int> check_out_point_shadow();
	void check_in_point_shadow(int index);
	
	[[nodiscard]] RID get_point_shadow_texture(int index) const;
	void set_point_shadow(int index, PointShadow const &shadow);

	void start_writing_point_lights();
	void stop_writing_point_lights();

	[[nodiscard]] std::optional<int> check_out_point_light();
	void check_in_point_light(int index);
	
	[[nodiscard]] std::optional<int> check_out_spot_shadow();
	void check_in_spot_shadow(int index);

	void set_point_light(int index, PointLight const &light);

	void prerender();

	void dispose() override;
	[[nodiscard]] bool disposed() const override;
};
