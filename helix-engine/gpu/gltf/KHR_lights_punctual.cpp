#include "json.hpp"
#include "KHR_lights_punctual.hpp"

#include "util.hpp"

using namespace gltf;
using namespace gltf::khr;
using namespace gltf::khr::lights_punctual;
using namespace simdjson;
using namespace simdjson::ondemand;

using _STD string_view;
using _STD string;
using _STD optional;
using _STD nullopt;


global lights_punctual::parse_ext_global(value &object) {
	global data{};

	simdjson_result<array> lights_array = object["lights"].get_array();
	data.lights.reserve(lights_array.count_elements());
	for (auto entry : lights_array) {
		string name = stringValue(object["name"], "");
		
		vec3 color = vec3Value(object["color"], vec3(1.0f));

		float intensity = ezGet<f32>(object["intensity"], 1.0f);
		
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
	
	return data;
}

node lights_punctual::parse_ext_node(value &object) {
	return { .light = ezGet(object["light"], 0) };
}