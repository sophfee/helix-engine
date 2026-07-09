#version 460 core

#extension GL_ARB_gpu_shader_int64 : require
#extension GL_NV_gpu_shader5 : require

#pragma include "shaders/static_buffers.glsl"

out InOutData {
    vec3 Position;
    vec3 Normal;
    vec3 Tangent;
    vec2 UV;
} vs_out;

void main() {
    DrawElementsIndirectCommand cmd = drawCommandSSBO.drawCommands[gl_DrawID];
    uint meshInstanceId = cmd.BaseInstance;
    GpuMeshInstance meshInstance = meshInstanceSSBO.meshInstances[meshInstanceId];
    GpuMeshTransform meshTransform = meshTransformSSBO.meshTransforms[meshInstance.MeshTransformID];
    uint meshId = meshInstance.MeshID;
    GpuMesh mesh = meshSSBO.meshes[meshId];
    
    uint16_t vertexIndex = indexSSBO.indices[gl_VertexID];
    Vertex   vertex = vertexSSBO.vertices[int(mesh.VertexOffset + uint64_t(vertexIndex))];
    
    vec3 Position = Unpack(vertex.Position);
    
    vec4 worldPosition = meshTransform.Model * vec4(Position, 1.0);
    vs_out.Position = worldPosition.xyz;
    vs_out.Normal = normalize((meshTransform.Model * vec4(Unpack(vertex.Normal), 0.0)).xyz);
    vs_out.Tangent = normalize((meshTransform.Model * vec4(Unpack(vertex.Tangent).xyz, 0.0)).xyz);
    vs_out.UV = Unpack(vertex.UV0);
    gl_Position = frameDataSSBO.frameData.ProjView * worldPosition;
}