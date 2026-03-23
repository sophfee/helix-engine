#include "json.hpp"
#include "KHR_lights_punctual.hpp"



using _STD string_view;
using _STD string;
using _STD optional;
using _STD nullopt;
using namespace gltf;
using namespace gltf::khr;
using namespace gltf::khr::lights_punctual;
using namespace simdjson;
using namespace simdjson::ondemand;
		

node lights_punctual::parse_ext_node(value &object) {
	return { .light = ezGet(object["light"], 0) };
}