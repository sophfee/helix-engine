#pragma once

#include "imgui/imgui.h"
#include <any>

namespace ImGui {
	bool InputAny(
		const char* label,
		std::any& value,
		ImGuiInputTextFlags flags = 0,
		ImGuiInputTextCallback callback = nullptr,
		void* user_data = nullptr
	);
}
