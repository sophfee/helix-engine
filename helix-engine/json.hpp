#include "simdjson/simdjson.h"
#include "math.hpp"

extern std::string getNullTerminatedString(simdjson::simdjson_result<simdjson::ondemand::value> value);
extern std::string stringValue(simdjson::simdjson_result<simdjson::ondemand::value> const &field, std::string fallback = "");
extern vec3 vec3Value(simdjson::simdjson_result<simdjson::ondemand::value> field, vec3 const &fallback = vec3(1.0f));

template <typename T>
inline T ezGet(simdjson::simdjson_result<simdjson::ondemand::value> value, T fallback) {
	if (value.has_value())
		return value.get<T>().value();
	return fallback;
}