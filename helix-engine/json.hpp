#pragma once
#include "simdjson/simdjson.h"
#include "math.hpp"

#include "types.hpp"

__forceinline std::string json_get_string_null_terminated(simdjson::simdjson_result<simdjson::ondemand::value> value) {
	std::string_view const view = value.get_string();
	return {view.data(), view.size()};
}

__forceinline std::string json_get_string(simdjson::simdjson_result<simdjson::ondemand::value> const &field, std::string fallback = "") {
	if (field.has_value()) return json_get_string_null_terminated(field);
	return fallback;
}

__forceinline vec3 json_get_float3(simdjson::simdjson_result<simdjson::ondemand::value> field, vec3 const &fallback = vec3(1.0f)) {
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



template <typename T>
__forceinline  T json_get(simdjson::simdjson_result<simdjson::ondemand::value> value, T fallback) {
	if (value.has_value())
		return value.get<T>().value();
	return fallback;
}