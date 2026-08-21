#pragma once

#include "types.hpp"
#include "math.hpp"

struct Frustum {
	float4 planes[6];
	Frustum() = default;
	inline explicit Frustum(const float4x4& viewProj) {
		const float4 row0(viewProj[0][0], viewProj[1][0], viewProj[2][0], viewProj[3][0]);
		const float4 row1(viewProj[0][1], viewProj[1][1], viewProj[2][1], viewProj[3][1]);
		const float4 row2(viewProj[0][2], viewProj[1][2], viewProj[2][2], viewProj[3][2]);
		const float4 row3(viewProj[0][3], viewProj[1][3], viewProj[2][3], viewProj[3][3]);

		planes[0] = row3 + row0; // Left
		planes[1] = row3 - row0; // Right
		planes[2] = row3 + row1; // Bottom	
		planes[3] = row3 - row1; // Top
		planes[4] = row3 + row2;        // Near
		planes[5] = row3 - row2; // Far

		for (float4 &plane : planes) {
			const float length = glm::length(float3(plane));
			plane /= length;
		}
	}
};