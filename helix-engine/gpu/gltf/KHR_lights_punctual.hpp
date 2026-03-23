// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md

#pragma once

#include <numbers>


#include "json.hpp"
#include "util.hpp"
#include "math.hpp"
#include "types.hpp"
#include "simdjson/simdjson.h"
namespace gltf {
	
	struct data;
	namespace khr::lights_punctual {
	constexpr auto name = "KHR_lights_punctual";
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

		using namespace gltf;
		using namespace gltf::khr;
		using namespace gltf::khr::lights_punctual;
		using namespace simdjson;
		using namespace simdjson::ondemand;
		using _STD string_view;
		using _STD string;
		using _STD optional;
		using _STD nullopt;
		__forceinline global parse_ext_global(simdjson::ondemand::value &object) {
			global data{};
			data.lights.reserve(512);
			auto lights_array = object["lights"];
	
			for ( auto entry : lights_array.get_array()) {
				string name = ::stringValue(object["name"], "");
		
				vec3 color = ::vec3Value(object["color"], vec3(1.0f));

				float intensity = ::ezGet<f32>(object["intensity"], 1.0f);
		
				string type_string = stringValue(object["type"], "point");
				auto light_type = light_type::point;
				switch (hash(type_string)) {
					case hash("spot"): light_type = light_type::spot;
					case hash("directional"): light_type = light_type::directional;
					default: break;
				}
		
				float range = ezGet<f32>(object["range"], 0.0f);
		
				data.lights.emplace_back(name, color, intensity, light_type, range);
			}

			data.lights.shrink_to_fit(); // reduce allocated memory.
	
			return data;
		}
		extern	 node parse_ext_node(simdjson::ondemand::value &object);
	}
}
