#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::uvec2;
using glm::uvec3;
using glm::uvec4;
using glm::mat3;
using glm::mat4;
using glm::quat;

using float2 = vec2;
using float3 = vec3;
using float4 = vec4;
using int2 = ivec2;
using int3 = ivec3;
using int4 = ivec4;
using uint2 = uvec2;
using uint3 = uvec3;
using uint4 = uvec4;
using float3x3 = mat3;
using float4x4 = mat4;

inline void print_matrix(mat4 const &m) {
	std::cout << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << "\n";
	std::cout << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << "\n";
	std::cout << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << "\n";
	std::cout << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << "\n";
}