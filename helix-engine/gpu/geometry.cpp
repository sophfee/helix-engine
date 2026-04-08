#include "geometry.hpp"

#include <algorithm>

#include "ecs/transform.h"


f32 Plane::signedDistance(vec3 const &v) const {
	return glm::dot(normal, v) - distance;
}

Frustum createFrustumFromCamera(vec3 const &position, vec3 const &forward, vec3 const &up, f32 fovY, f32 aspectRatio, f32 nearZ, f32 farZ) {
	Frustum result;

	vec3 const right = glm::normalize(glm::cross(forward, up));
	vec3 const upNorm = glm::normalize(up);
	vec3 const forwardNorm = glm::normalize(forward);

	f32 const halfVSide = farZ * tanf(fovY * .5f);
	f32 const halfHSide = halfVSide * aspectRatio;
	vec3 const frontMultFar = farZ * forward;

	result.nearFace 	= { position + nearZ * forward, forward };
	result.farFace 		= { position + frontMultFar, -forward };
	result.rightFace 	= { position, glm::cross(frontMultFar - right * halfHSide, up) };
	result.leftFace 	= { position, glm::cross(up, frontMultFar + right * halfHSide) };
	result.topFace 		= { position, glm::cross(right, frontMultFar - up * halfVSide) };
	result.bottomFace 	= { position, glm::cross(frontMultFar + up * halfVSide, right) };
	
	return result;
}

Frustum createFrustumFromMatrix(mat4 const &view, f32 const fovY, f32 const aspectRatio, f32 const nearZ, f32 const farZ) {
	vec3 const position(view[0][2], view[1][2], view[2][2]);
	vec3 const forward(-view[2]);
	vec3 const up(view[1]);
	return createFrustumFromCamera(position, forward, up, fovY, aspectRatio, nearZ, farZ);
}

#define ON_FRUSTUM(OBJECT, FRUSTUM) \
	 (OBJECT).forwardPlane((FRUSTUM).leftFace) && \
	 (OBJECT).forwardPlane((FRUSTUM).rightFace) && \
	 (OBJECT).forwardPlane((FRUSTUM).farFace) &&  \
	((OBJECT).forwardPlane((FRUSTUM).nearFace) && \
	 (OBJECT).forwardPlane((FRUSTUM).topFace) && \
	 (OBJECT).forwardPlane((FRUSTUM).bottomFace)) \

bool Sphere::onFrustum(Frustum const &frustum, Transform const &model) const {
	vec3 global_scale = model.scale;
	vec3 const global_center(model.matrix() * vec4(center, 1.0f));

	f32 const max_scale = std::max({global_scale.x, global_scale.y, global_scale.z});

	Sphere const global_sphere(
		global_center,
		radius * (max_scale * 0.5f)
	);

	return global_sphere.forwardPlane(frustum.leftFace) &&
	       global_sphere.forwardPlane(frustum.rightFace) &&
	       global_sphere.forwardPlane(frustum.farFace) &&
	       global_sphere.forwardPlane(frustum.nearFace) &&
	       global_sphere.forwardPlane(frustum.topFace) &&
	       global_sphere.forwardPlane(frustum.bottomFace);
}


bool Sphere::forwardPlane(Plane const &plane) const {
	return plane.signedDistance(center) > -radius;
}

Array<vec3, 8> AABB::vertices() const {
	return {
		vec3{ center.x - extents.x, center.y - extents.y, center.z - extents.z },
		vec3{ center.x + extents.x, center.y - extents.y, center.z - extents.z },
		vec3{ center.x - extents.x, center.y + extents.y, center.z - extents.z },
		vec3{ center.x + extents.x, center.y + extents.y, center.z - extents.z },
		vec3{ center.x - extents.x, center.y - extents.y, center.z + extents.z },
		vec3{ center.x + extents.x, center.y - extents.y, center.z + extents.z },
		vec3{ center.x - extents.x, center.y + extents.y, center.z + extents.z },
		vec3{ center.x + extents.x, center.y + extents.y, center.z + extents.z }
	};
}

bool AABB::onFrustum(Frustum const &frustum, Transform const &model) const {
	vec3 const global_center( model.matrix() * vec4(center, 1.0f) );

	vec3 const right(model.right() * extents.x);
	vec3 const up(model.up() * extents.y);
	vec3 const forward(model.forward() * extents.z);

	f32 const newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
	                    std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
	                    std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

	f32 const newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
	                    std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
	                    std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

	f32 const newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
	                    std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
	                    std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

	AABB const globalAABB(global_center, newIi, newIj, newIk);

	return globalAABB.forwardPlane(frustum.leftFace) &&
	       globalAABB.forwardPlane(frustum.rightFace) &&
	       globalAABB.forwardPlane(frustum.topFace) &&
	       globalAABB.forwardPlane(frustum.bottomFace) &&
	       globalAABB.forwardPlane(frustum.nearFace) &&
	       globalAABB.forwardPlane(frustum.farFace);
}

bool AABB::forwardPlane(Plane const &plane) const {
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	f32 const r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
				  extents.z * std::abs(plane.normal.z);
	
	return -r <= plane.signedDistance(center);
}