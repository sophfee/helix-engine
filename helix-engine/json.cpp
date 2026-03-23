#include "json.hpp"

#include "types.hpp"

std::string getNullTerminatedString(simdjson::simdjson_result<simdjson::ondemand::value> value) {
	std::string_view const view = value.get_string();
	return {view.data(), view.size()};
}

std::string stringValue(simdjson::simdjson_result<simdjson::ondemand::value> const &field, std::string fallback) {
	if (field.has_value()) return getNullTerminatedString(field);
	return fallback;
}

vec3 vec3Value(simdjson::simdjson_result<simdjson::ondemand::value> field, vec3 const &fallback) {
	vec3 result = fallback;
	
	if (!field.has_value()) return result;
	
	if (simdjson::simdjson_result<simdjson::ondemand::array> arr = field.get_array(); arr.has_value()) {
		for (int i = 0; simdjson::simdjson_result co : arr) {
			if (i > 2) break;
			result[i] = co.get<f32>().value();
			++i;
		}
	}
	
	return result;
}

