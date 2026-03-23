// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md

#pragma once

#include <numbers>

#include "math.hpp"
#include "types.hpp"
#include "simdjson/simdjson.h"

namespace gltf {
	struct data;
	namespace khr::lights_punctual {
		enum class light_type : u8 {
			directional,
			point,
			spot,
		};

		struct spot {
			::f32 inner_cone_angle = 0.0f;
			::f32 outer_cone_angle = _STD numbers::pi_v<::f32> / 4.0f;
		};

		struct light {
			_STD string name;
			::vec3 color;
			::f32 intensity;
			light_type type;
			::f32 range;
			_STD optional<spot> spot;
		};
	
		struct global {
			_STD vector<light> lights;
		};

		struct node {
			i32 light;
		};

		extern global parse_ext_global(simdjson::ondemand::value &object);
		extern	 node parse_ext_node(simdjson::ondemand::value &object);
	}
}
