#pragma once
#include "math.hpp"
#include "types.hpp"

class Mesh;
class Primitive {
public:

	static SharedPtr<Mesh> makeQuad(vec2 const &size, bool flip_y = false);
};
