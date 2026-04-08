#pragma once

#include "types.hpp"
#include "math.hpp"

namespace rd {
	static u32 full_screen_quad;
}

class Transform;

struct Plane {
	vec3 normal;
	f32 distance;

	Plane() = default;
	Plane(vec3 const &p, vec3 const &n) :
		normal(glm::normalize(n)),
		distance(glm::dot(normal, p))
	{}

	_NODISCARD f32 signedDistance(vec3 const &v) const;
};

struct Frustum {
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;
};

extern Frustum createFrustumFromCamera(
	vec3 const &position,
	vec3 const &forward,
	vec3 const &up,
	f32 fovY,
	f32 aspectRatio,
	f32 nearZ,
	f32 farZ
);

extern Frustum createFrustumFromMatrix(
	mat4 const &view,
	f32 fovY,
	f32 aspectRatio,
	f32 nearZ,
	f32 farZ
);

struct Bounds {
	virtual ~Bounds() = default;

	_NODISCARD virtual bool onFrustum(Frustum const &frustum, Transform const &model) const = 0;
	_NODISCARD virtual bool forwardPlane(Plane const &plane) const = 0;
};

struct Sphere : public Bounds {
	vec3 center{ 0.0f, 0.0f, 0.0f };
	f32 radius{ 0.0f };

	Sphere(vec3 const &center, f32 const radius) : center(center), radius(radius) {}
	
	_NODISCARD bool forwardPlane(Plane const &plane) const final;
	_NODISCARD bool onFrustum(Frustum const &frustum, Transform const &model) const final;
};

struct AABB : public Bounds {
	vec3 center{ 0.0f, 0.0f, 0.0f };
	vec3 extents{ 0.0f, 0.0f, 0.0f };

	AABB(vec3 const &min, vec3 const &max)
		: center( (max + min) * 0.5f ), extents( max.x - center.x, max.y - center.y, max.z - center.z ) {}

	AABB(vec3 const &cen, f32 i, f32 j, f32 k) :
		center(cen), extents(i, j, k) {}

	_NODISCARD Array<vec3, 8> vertices() const;
	_NODISCARD bool onFrustum(Frustum const &frustum, Transform const &model) const override;
	_NODISCARD bool forwardPlane(Plane const &plane) const final;
};