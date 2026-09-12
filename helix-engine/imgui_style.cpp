#include <imgui.h>
#include "imgui_style.hpp"

#include "types.hpp"

namespace {
	constexpr ImVec4 mix(const ImVec4& a, const ImVec4& b, const float t) {
		return {
			a.x * (1.0f - t) + b.x * t,
			a.y * (1.0f - t) + b.y * t,
			a.z * (1.0f - t) + b.z * t,
			a.w * (1.0f - t) + b.w * t
		};
	}
	
	ImVec4 desaturate(const ImVec4& color) {
		// Standard ITU-R BT.709 weights for human luminance perception
		float gray = 0.2126f * color.x + 0.7152f * color.y + 0.0722f * color.z;
		return { gray, gray, gray, color.w };
	}
	
	ImVec4 saturate(const ImVec4& color, const float saturation) {
		float gray = 0.2126f * color.x + 0.7152f * color.y + 0.0722f * color.z;
		return {
			gray + (color.x - gray) * saturation,
			gray + (color.y - gray) * saturation,
			gray + (color.z - gray) * saturation,
			color.w
		};
	}
	
	constexpr ImVec4 color(const u8 r, const u8 g, const u8 b, const u8 a) {
		return {
			r / 255.0f,
			g / 255.0f,
			b / 255.0f,
			a / 255.0f
		};
	}
	
	constexpr ImVec4 with_alpha(const ImVec4& color, const u8 alpha) {
		return {
			color.x,
			color.y,
			color.z,
			alpha / 255.0f
		};
	}
	
	void theme_colors(ImGuiStyle& style, ImVec4* colors) {
		constexpr ImVec4 primary_main = color(176, 47, 194, 255);
		constexpr ImVec4 primary_active = color(234, 141, 255, 255);
		constexpr ImVec4 primary_hovered = color(219, 52, 242, 255);
		
		constexpr ImVec4 dark_0 = color(28, 28, 30, 255);
		constexpr ImVec4 dark_1 = color(36, 36, 38, 255);
		constexpr ImVec4 dark_2 = color(68, 68, 70, 255);
		constexpr ImVec4 dark_3 = color(84, 84, 86, 255);
		constexpr ImVec4 dark_4 = color(99, 99, 102, 255);
		constexpr ImVec4 dark_5 = color(124, 124, 126, 255);
		
		constexpr ImVec4 light_0 = color(235, 235, 240, 255);
		
		colors[ImGuiCol_Text] = light_0;
		colors[ImGuiCol_TextDisabled] = dark_5;
		
		colors[ImGuiCol_WindowBg] = with_alpha(dark_0, 230);
		colors[ImGuiCol_ChildBg] = with_alpha(dark_1, 20);
		colors[ImGuiCol_PopupBg] = with_alpha(dark_2, 200);
		
		colors[ImGuiCol_Border] = with_alpha(mix(primary_main, dark_5, .5f), 200);
		colors[ImGuiCol_BorderShadow] = with_alpha(saturate(primary_main, .6f), 100);
		
		colors[ImGuiCol_FrameBg] = with_alpha(primary_main, 170);
		colors[ImGuiCol_FrameBgHovered] = primary_hovered;
		colors[ImGuiCol_FrameBgActive] = primary_active;
		
		colors[ImGuiCol_TitleBg] = primary_hovered;
		colors[ImGuiCol_TitleBgActive] = primary_main;
		colors[ImGuiCol_TitleBgCollapsed] = with_alpha(saturate(primary_main, .2f), 200);
		
		colors[ImGuiCol_MenuBarBg] = mix(primary_main, dark_1, 0.5f);
		
		colors[ImGuiCol_ScrollbarBg] = with_alpha(dark_2, 100);
		colors[ImGuiCol_ScrollbarGrab] = with_alpha(primary_main, 150);
		colors[ImGuiCol_ScrollbarGrabHovered] = with_alpha(primary_hovered, 200);
		colors[ImGuiCol_ScrollbarGrabActive] = with_alpha(primary_active, 200);
		
		colors[ImGuiCol_CheckMark] = primary_active;
		colors[ImGuiCol_CheckboxSelectedBg] = with_alpha(primary_main, 150);
		
		colors[ImGuiCol_Button] = with_alpha(primary_main, 150);
		colors[ImGuiCol_ButtonHovered] = primary_hovered;
		colors[ImGuiCol_ButtonActive] = primary_active;
		
		colors[ImGuiCol_Header] = with_alpha(primary_main, 150);
		colors[ImGuiCol_HeaderHovered] = primary_hovered;
		colors[ImGuiCol_HeaderActive] = primary_active;
		
		colors[ImGuiCol_Separator] = with_alpha(mix(primary_main, dark_1, .2f), 200);
		colors[ImGuiCol_SeparatorHovered] = with_alpha(primary_hovered, 200);
		colors[ImGuiCol_SeparatorActive] = with_alpha(primary_active, 200);
		
		colors[ImGuiCol_ResizeGrip] = with_alpha(primary_main, 150);
		colors[ImGuiCol_ResizeGripHovered] = primary_hovered;
		colors[ImGuiCol_ResizeGripActive] = primary_active;
		
		colors[ImGuiCol_InputTextCursor] = with_alpha(primary_main, 200);
		colors[ImGuiCol_Tab] = with_alpha(primary_main, 150);
		colors[ImGuiCol_TabHovered] = primary_hovered;
		colors[ImGuiCol_TabActive] = primary_active;
		colors[ImGuiCol_TabUnfocused] = with_alpha(mix(primary_main, dark_0, .2f), 200);
		colors[ImGuiCol_TabUnfocusedActive] = with_alpha(mix(primary_main, dark_1, .2f), 200);
		
		colors[ImGuiCol_TableHeaderBg] = with_alpha(primary_main, 150);
		colors[ImGuiCol_TableBorderStrong] = with_alpha(saturate(primary_main, .6f), 120);
		colors[ImGuiCol_TableBorderLight] = with_alpha(saturate(primary_main, .1f), 120);
		colors[ImGuiCol_TableRowBg] = with_alpha(dark_1, 165);
		colors[ImGuiCol_TableRowBgAlt] = with_alpha(dark_2, 165);
	}
	
	void style_vars(ImGuiStyle& style) {
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.FrameBorderSize = 1.0f;
		style.ImageBorderSize = 1.0f;
		
		constexpr auto default_rounding = 1.0f;
		style.WindowRounding = default_rounding;
		style.ChildRounding = default_rounding;
		style.FrameRounding = default_rounding;
		style.DragDropTargetRounding = default_rounding;
		style.GrabRounding = default_rounding;
		style.ImageRounding = default_rounding;
		style.MenuItemRounding = default_rounding;
		style.PopupRounding = default_rounding;
		style.ScrollbarRounding = default_rounding;
		style.SelectableRounding = default_rounding;
		style.TabRounding = default_rounding;
		style.TreeLinesRounding = default_rounding;
		
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	}
}

void setup_helix_imgui_theme() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	theme_colors(style, colors);
	style_vars(style);
}
