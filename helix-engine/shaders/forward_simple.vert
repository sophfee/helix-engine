#version 460 core

#pragma include "shaders/static_buffers.glsl"

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Tangent;
layout (location = 3) in vec2 a_TexCoord;

out InOutData
{
    vec3 Position;
    vec3 Normal;
    vec3 Tangent;
    vec2 TexCoord;
} v_out;

out flat int Handedness;

void main()
{
    FrameData frameData = GetFrameData();
    
    vec3 v_Position = (frameData.ProjView * vec4(a_Position, 1.0)).xyz;
    vec3 v_Normal   = normalize(frameData.InvView * vec4(a_Normal, 0.0)).xyz;
    vec3 v_Tangent  = normalize(frameData.InvView * vec4(a_Tangent.xyz, 0.0)).xyz;
    vec2 v_TexCoord = a_TexCoord;
    
    v_out.Position  =  v_Position;
    v_out.Normal    =  v_Normal;
    v_out.Tangent   =  v_Tangent;
    v_out.TexCoord  =  v_TexCoord;
    
    Handedness = int(a_Tangent.w);
}