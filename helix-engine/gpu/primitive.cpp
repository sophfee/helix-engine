#include "primitive.hpp"

#include "buffer.h"
#include "graphics.hpp"
#include "mesh.hpp"

SharedPtr<Mesh> Primitive::makeQuad(vec2 const &size, bool flip_y) {
	auto vertex_array = std::make_shared<VertexArray>();
	auto const vertex_buffer = std::make_shared<TypedBuffer<vec2>>();
	auto const index_buffer = std::make_shared<TypedBuffer<uint8_t>>();

	vec2 const half_size(size / 2.0f);

	Array<vec2, 4> const vertices{
		vec2(-half_size.x, -half_size.y),
		vec2(-half_size.x,  half_size.y),
		vec2( half_size.x,  half_size.y),
		vec2( half_size.x, -half_size.y),
	};

	constexpr Array<uint8_t, 6> indices{
		0, 1, 2,
		2, 3, 0
	};

	vertex_buffer->allocateElements(vertices);
	index_buffer->allocateElements(indices);

	vertex_array->setAttribute({
		.index = 0,
		.binding = 0,
		.size = 2,
		.stride = sizeof(f32) * 2,
		.offset = 0,
		.type = EComponentType::SINGLE_FLOAT
	});

	vertex_array->setVertexBuffer(0, *vertex_buffer, sizeof(f32) * 2, 0);
	vertex_array->setElementBuffer(*index_buffer);

	vertex_array->elements_count = 6;

	auto const mesh = std::make_shared<Mesh>();
	mesh->addPrimitive(vertex_array, nullptr, AABB{vec3(-half_size.x, -half_size.y, 0.0f), vec3(half_size.x, half_size.y, 0.0f)});
	mesh->addBuffer(vertex_buffer);
	mesh->addBuffer(index_buffer);
	
	return mesh;
}