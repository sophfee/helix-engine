
#ifndef NO_BINDLESS
#extension GL_ARB_bindless_texture : require
#endif

struct FrameData {
    mat4 ProjView;
    mat4 View;
    mat4 InvView;
    mat4 Projection;
    mat4 InvProjection;
    mat4 InvProjView;
    
    float Near;
    float Far;
    float Time;
};

#ifdef NO_BINDLESS

uniform sampler2D BaseColorTextures[16];
uniform sampler2D MetallicRoughnessTextures[16];
uniform sampler2D NormalTextures[16];
uniform sampler2D EmissiveTextures[16];

struct Material {
    vec4 BaseColorFactor;
    vec3 EmissiveFactor;
    
    int AlphaMode; //< 0 = Opaque, 1 = Mask, 2 = Blend
    
    float RoughnessFactor;
    float MetallicFactor;
    float AlphaCutoff;
    
    float _pad1;
    
    int BaseColorTextureIndex; // -1 if no texture
    int MetallicRoughnessTextureIndex; // -1 if no texture
    int NormalTextureIndex; // -1 if no texture
    int EmissiveTextureIndex; // -1 if no texture
};

vec4 SampleBaseColor(Material material, vec2 uv) {
    if (material.BaseColorTextureIndex == -1) {
        return material.BaseColorFactor;
    } else {
        return texture(BaseColorTextures[material.BaseColorTextureIndex], uv) * material.BaseColorFactor;
    }
}

vec2 SampleMetallicRoughness(Material material, vec2 uv) {
    if (material.MetallicRoughnessTextureIndex == -1) {
        return vec2(material.MetallicFactor, material.RoughnessFactor);
    } else {
        vec4 mrSample = texture(MetallicRoughnessTextures[material.MetallicRoughnessTextureIndex], uv);
        return vec2(mrSample.b * material.MetallicFactor, mrSample.g * material.RoughnessFactor);
    }
}

vec3 SampleNormal(Material material, vec2 uv) {
    if (material.NormalTextureIndex == -1) {
        return vec3(0.0, 0.0, 1.0);
    } else {
        vec3 normalSample = texture(NormalTextures[material.NormalTextureIndex], uv).xyz * 2.0 - 1.0;
        return normalSample;
    }
}

vec3 SampleEmissive(Material material, vec2 uv) {
    if (material.EmissiveTextureIndex == -1) {
        return material.EmissiveFactor;
    } else {
        return texture(EmissiveTextures[material.EmissiveTextureIndex], uv).rgb * material.EmissiveFactor;
    }
}

#else

struct Material {
    vec4 BaseColorFactor;
    vec3 EmissiveFactor;
    
    int AlphaMode; //< 0 = Opaque, 1 = Mask, 2 = Blend
    
    float RoughnessFactor;
    float MetallicFactor;
    float AlphaCutoff;
    
    float _pad1;
    
    sampler2D BaseColor;
    sampler2D MetallicRoughness;
    sampler2D Normal;
    sampler2D Emissive;
};

vec4 SampleBaseColor(Material material, vec2 uv) {
    return texture(material.BaseColor, uv) * material.BaseColorFactor;
}

vec2 SampleMetallicRoughness(Material material, vec2 uv) {
    vec4 mrSample = texture(material.MetallicRoughness, uv);
    return vec2(mrSample.b * material.MetallicFactor, mrSample.g * material.RoughnessFactor);
}

vec3 SampleNormal(Material material, vec2 uv) {
    vec3 normalSample = texture(material.Normal, uv).xyz * 2.0 - 1.0;
    return normalSample;
}

vec3 SampleEmissive(Material material, vec2 uv) {
    return texture(material.Emissive, uv).rgb * material.EmissiveFactor;
}

#endif

struct Mesh {
    mat4 ModelMatrix;
    mat4 InvModelMatrix;
};

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

//< Bindless and non-bindless versions of the shadow structs are identical in size, this is good for an optimized and consistent allocation.
#ifdef NO_BINDLESS

//< Using sampler array types allows ambiguity in the size of the array, which is good.
uniform samplerCubeArrayShadow PointShadowMaps;
uniform sampler2DArrayShadow SpotShadowMaps;

struct PointShadow {
    uvec2 _pad0;
    int ShadowTextureIndex; // -1 if no shadow map, otherwise index into array of shadow maps
    int _pad1;
    mat4 LightViewProj[6];
    
    vec3 Position;
    int LightIndex; //< Index into array of point lights, this allows the two to be associated without needing to duplicate the light data in the shadow struct
    
    float NearPlane;
    float FarPlane;
};

float SamplePointShadow(PointShadow ps, vec3 dir, float compare, float bias) {
    return texture(PointShadowMaps, vec4(vec3(dir.xy, dir.z - bias), ps.ShadowTextureIndex), compare);
}

float DebugSamplePointShadow(PointShadow ps, vec3 dir) {
    return texture(samplerCubeArray(PointShadowMaps), vec4(dir.xyz, ps.ShadowTextureIndex)).r;
}

struct SpotShadow {
    uvec2 _pad0;
    int ShadowTextureIndex; // -1 if no shadow map, otherwise index into array of shadow maps
    int _pad1;
    mat4 LightViewProj;
    
    vec3 Position;
    int LightIndex; //< Index into array of spot lights, this allows the two to be associated without needing to duplicate the light data in the shadow struct
    
    float NearPlane;
    float FarPlane;
};

#else

struct PointShadow {
    samplerCubeShadow ShadowTexture;
    uvec2 _pad0;
    mat4 LightViewProj[6];
    
    vec3 Position;
    int LightIndex; //< Index into array of point lights, this allows the two to be associated without needing to duplicate the light data in the shadow struct
    
    float NearPlane;
    float FarPlane;
};

float SamplePointShadow(PointShadow ps, vec3 dir, float compare, float bias) {
    return texture(ps.ShadowTexture, vec4(dir, compare - bias));
}

float DebugSamplePointShadow(PointShadow ps, vec3 dir) {
    return texture(samplerCube(ps.ShadowTexture), dir).r;
}

struct SpotShadow {
    sampler2DShadow ShadowTexture;
    uvec2 _pad0;
    mat4 LightViewProj;
    
    vec3 Position;
    int LightIndex; //< Index into array of spot lights, this allows the two to be associated without needing to duplicate the light data in the shadow struct
    
    float NearPlane;
    float FarPlane;
};

#endif