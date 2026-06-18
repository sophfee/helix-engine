#version 460 core

#pragma include "shaders/static_buffers.glsl"

noperspective in vec4 FragPos;

uniform int lightIndex;

void main() {
    PointShadow pointShadow = GetPointShadow(lightIndex);
    //PointLight pointLight = GetPointLight(lightIndex);
    //
    vec4 projected = pointShadow.LightViewProj[gl_Layer] * FragPos;
    // boingo
    float depth = length(FragPos.xyz - pointShadow.Position);
    depth /= pointShadow.FarPlane;
    gl_FragDepth = depth; // (projected.z) / pointShadow.FarPlane; // / projected.w;
}