#version 460 core

#pragma include "shaders/static_buffers.glsl"

layout (triangles) in;

layout (triangle_strip, max_vertices = 18) out;

uniform int lightIndex;

noperspective out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle vertex
        {
            PointShadow ps = GetPointShadow(lightIndex);
            FragPos = gl_in[i].gl_Position; // Pass the world space position to the fragment shader
            gl_Position = ps.LightViewProj[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}