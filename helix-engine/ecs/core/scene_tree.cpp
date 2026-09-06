// ReSharper disable CppTooWideScopeInitStatement
#include "scene_tree.hpp"

#include <cassert>

#include "component.hpp"
#include "imgui_internal.h"
#include "engine/filesystem.hpp"
#include "gpu/graphics.hpp"
#include "gpu/window.hpp"

//
// SceneTree
//

SceneTree::SceneTree(SharedPtr<Window> const &window)
	: window_(window) {
}

SceneTree::~SceneTree() {
	//< Nothing as of now.
	dispose();
}

Result<RID> SceneTree::create_entity() {
	Entity entity(shared_from_this(), _STD nullopt, 0);
	SlotPool<Entity>::Handle handle = entities_.emplace(std::move(entity));
	handle->scene_tree_ = shared_from_this();
	handle->unique_id_ = handle;
	handle->parent_id_ = root_id_;
	handle->is_root_ = false;
	
	return {handle};
}

Error SceneTree::destroy_entity(RID id) {
	// Get entity there
	
	Entity* entity = entities_.get(id);
	
	if (entity->is_destroyed_)
		return OK; // It's ok :]
	
	entity->is_enabled_ = false;
	entity->is_destroyed_ = true;
	
	Error err = OK;
	for (RID const cid : entity->children_) {
		err = destroy_entity(cid); // Recursive freeing.
		assert(err == OK);
	}

	entity->children_.clear();
	entity->children_.shrink_to_fit();

	for (const GLID rid : entity->components_) {
		IComponentProvider::ProviderComponent* pc = IComponentProvider::provider_components.get(rid.global);//.provider->removeFrom(entity->id());
		IComponentProvider** p = IComponentProvider::providers.get(pc->provider);
		(*p)->remove_from(entity->get_id());
	}

	entity->components_.clear();
	entity->components_.shrink_to_fit();

	// Remove myself from my parent

	entity->name_ = "deleted ent";
	
	assert(entities_.erase(id));
	
	return OK;
}
void SceneTree::set_root(RID const root_rid) {
	if (root_id_ != RID{0, 0})
		entities_.get(root_id_)->is_root_ = false;
	entities_.get(root_rid)->is_root_ = true;
	root_id_ = root_rid;
}

Entity* SceneTree::get_entity(const RID entity_rid) {
	return entities_.get(entity_rid);
}

const Entity *SceneTree::get_entity(const RID entity_rid) const {
	return entities_.get(entity_rid);
}

Vector<Entity*> SceneTree::get_entities() const {
	Vector<Entity*> result;
	for (auto ent : entities_)
		result.push_back(std::addressof(const_cast<Entity &>(ent.second.get())));
	return result;
}

void SceneTree::init_frame(f64 delta_time) {
	delta_time_ = delta_time;
	visit_component([](Component *component, f64 const delta) {
		component->update(delta);
	}, root_id_, delta_time);
}
void SceneTree::init_draw(RenderPassInfo const &info) {
	setup_render_pass(info);
	visit_component([](Component *component, RenderPassInfo const &p_info) {
		component->draw(p_info);
	}, root_id_, info);
}

void SceneTree::init_render_setup(RenderPassInfo const &info) {
	setup_render_pass(info);
	gpu_check;
	
	visit_component([](Component *component, RenderPassInfo const &p_info) {
		component->render_setup(p_info);
	}, root_id_, info);
}

void SceneTree::send_mouse_event(MouseInputEvent const &event) {
	visit_component([](Component *component, MouseInputEvent const &ev) {
		component->mouse(ev);
	}, root_id_, event);
}

void SceneTree::render_extra_passes() {
	visit_component([this](Component *component) {
		Optional<RenderPassInfo> const pass_info = component->get_custom_render_pass();
		if (pass_info.has_value()) {
			setup_render_pass(pass_info.value());
			init_draw(pass_info.value());
			gpu_check;
		}
	}, root_id_);
	ivec4 const vp = get_window()->viewport();
	assert(vp.z > 0 && vp.w > 0);
}

void SceneTree::editor_draw_entity_hierarchy_recursive(Entity *entity) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_Framed;
	if (entity->children_.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	//ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(.33f, .5f, .9f).Value);
	size_t children_count = entity->children_.size();
	const String tree_node_id = entity->name_.size() > 0 ? entity->name_ + "##" + std::to_string(entity->unique_id_.upper) : "Entity##" + std::to_string(entity->unique_id_.upper);

	const bool open_node = ImGui::TreeNodeEx(tree_node_id.c_str(), flags, "%s (%zu)", entity->name_.size() > 0 ? entity->name_.c_str() : "Entity", children_count);
	//ImGui::PopStyleColor(1);
	
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
		scene_graph_selected_entity_ = entity->unique_id_;
	}
	
	if (open_node) {
		if (!entity->children_.empty())
			for (const auto id : entity->children_) {
				Entity *const child = get_entity(id);
				editor_draw_entity_hierarchy_recursive(child);
			}
		ImGui::TreePop();
	}
}

namespace {
#ifdef _DEBUG
	void SetupImGuiDraculaStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		// --- 1. Sizing and Spacing (Clean & Balanced) ---
		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(6.0f, 4.0f);
		style.ItemSpacing = ImVec2(8.0f, 6.0f);
		style.ScrollbarSize = 14.0f;
		style.GrabMinSize = 12.0f;

		// --- 2. Borders & Rounding ---
		style.WindowRounding = 6.0f;
		style.FrameRounding = 4.0f;
		style.PopupRounding = 4.0f;
		style.ScrollbarRounding = 12.0f;
		style.GrabRounding = 4.0f;
		style.TabRounding = 4.0f;

		style.WindowBorderSize = 1.0f;
		style.FrameBorderSize = 1.0f;

		// --- 3. The Dracula Color Palette ---
		// Background: #282a36 | Selection: #44475a | Foreground: #f8f8f2
		// Comment: #6272a4    | Cyan: #8be9fd      | Green: #50fa7b
		// Orange: #ffb86c     | Pink: #ff79c6      | Purple: #bd93f9
		// Red: #ff5555        | Yellow: #f1fa8c

		// Text
		colors[ImGuiCol_Text] = ImVec4(0.97f, 0.97f, 0.95f, 1.00f); // #f8f8f2
		colors[ImGuiCol_TextDisabled] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f); // #6272a4

		// Backgrounds
		colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f); // #282a36
		colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.21f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.16f, 0.21f, 0.96f);

		// Borders
		colors[ImGuiCol_Border] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f); // #44475a
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// Frames (Inputs, etc.)
		colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f); // #44475a
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f); // #6272a4
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.48f, 0.55f, 0.74f, 1.00f);

		// Title Bars
		colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f); // Darker
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);

		// Menus
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);

		// Scrollbars
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.55f, 0.74f, 1.00f);

		// Interactables
		colors[ImGuiCol_CheckMark] = ImVec4(0.31f, 0.98f, 0.48f, 1.00f); // #50fa7b (Green)
		colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.58f, 0.98f, 1.00f); // #bd93f9 (Purple)
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.84f, 0.68f, 1.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.47f, 0.78f, 1.00f); // #ff79c6 (Pink)
		colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.37f, 0.62f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.55f, 0.74f, 1.00f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);

		// Tables
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);

		// Misc
		colors[ImGuiCol_PlotLines] = ImVec4(0.55f, 0.91f, 0.99f, 1.00f); // #8be9fd (Cyan)
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.74f, 0.58f, 0.98f, 1.00f);

#ifdef IMGUI_HAS_DOCK
		colors[ImGuiCol_DockingPreview] = ImVec4(0.74f, 0.58f, 0.98f, 0.50f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
#endif
	}
	void setup_catppuccin_mocha_theme() {
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		// Catppuccin Mocha Palette
		// --------------------------------------------------------
		constexpr ImVec4 base       = ImVec4(0.117f, 0.117f, 0.172f, 1.0f); // #1e1e2e
		constexpr ImVec4 mantle     = ImVec4(0.109f, 0.109f, 0.156f, 1.0f); // #181825
		constexpr ImVec4 surface0   = ImVec4(0.200f, 0.207f, 0.286f, 1.0f); // #313244
		constexpr ImVec4 surface1   = ImVec4(0.247f, 0.254f, 0.337f, 1.0f); // #3f4056
		constexpr ImVec4 surface2   = ImVec4(0.290f, 0.301f, 0.388f, 1.0f); // #4a4d63
		constexpr ImVec4 overlay0   = ImVec4(0.396f, 0.403f, 0.486f, 1.0f); // #65677c
		constexpr ImVec4 overlay2   = ImVec4(0.576f, 0.584f, 0.654f, 1.0f); // #9399b2
		constexpr ImVec4 text       = ImVec4(0.803f, 0.815f, 0.878f, 1.0f); // #cdd6f4
		constexpr ImVec4 subtext0   = ImVec4(0.639f, 0.658f, 0.764f, 1.0f); // #a3a8c3
		constexpr ImVec4 mauve      = ImVec4(0.796f, 0.698f, 0.972f, 1.0f); // #cba6f7
		constexpr ImVec4 peach      = ImVec4(0.980f, 0.709f, 0.572f, 1.0f); // #fab387
		constexpr ImVec4 yellow     = ImVec4(0.980f, 0.913f, 0.596f, 1.0f); // #f9e2af
		constexpr ImVec4 green      = ImVec4(0.650f, 0.890f, 0.631f, 1.0f); // #a6e3a1
		constexpr ImVec4 teal       = ImVec4(0.580f, 0.886f, 0.819f, 1.0f); // #94e2d5
		constexpr ImVec4 sapphire   = ImVec4(0.458f, 0.784f, 0.878f, 1.0f); // #74c7ec
		constexpr ImVec4 blue       = ImVec4(0.533f, 0.698f, 0.976f, 1.0f); // #89b4fa
		constexpr ImVec4 lavender   = ImVec4(0.709f, 0.764f, 0.980f, 1.0f); // #b4befe

		// Main window and backgrounds
		colors[ImGuiCol_WindowBg]             = base;
		colors[ImGuiCol_ChildBg]              = base;
		colors[ImGuiCol_PopupBg]              = surface0;
		colors[ImGuiCol_Border]               = surface1;
		colors[ImGuiCol_BorderShadow]         = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_FrameBg]              = surface0;
		colors[ImGuiCol_FrameBgHovered]       = surface1;
		colors[ImGuiCol_FrameBgActive]        = surface2;
		colors[ImGuiCol_TitleBg]              = mantle;
		colors[ImGuiCol_TitleBgActive]        = surface0;
		colors[ImGuiCol_TitleBgCollapsed]     = mantle;
		colors[ImGuiCol_MenuBarBg]            = mantle;
		colors[ImGuiCol_ScrollbarBg]          = surface0;
		colors[ImGuiCol_ScrollbarGrab]        = surface2;
		colors[ImGuiCol_ScrollbarGrabHovered] = overlay0;
		colors[ImGuiCol_ScrollbarGrabActive]  = overlay2;
		colors[ImGuiCol_CheckMark]            = green;
		colors[ImGuiCol_SliderGrab]           = sapphire;
		colors[ImGuiCol_SliderGrabActive]     = blue;
		colors[ImGuiCol_Button]               = surface0;
		colors[ImGuiCol_ButtonHovered]        = surface1;
		colors[ImGuiCol_ButtonActive]         = surface2;
		colors[ImGuiCol_Header]               = surface0;
		colors[ImGuiCol_HeaderHovered]        = surface1;
		colors[ImGuiCol_HeaderActive]         = surface2;
		colors[ImGuiCol_Separator]            = surface1;
		colors[ImGuiCol_SeparatorHovered]     = mauve;
		colors[ImGuiCol_SeparatorActive]      = mauve;
		colors[ImGuiCol_ResizeGrip]           = surface2;
		colors[ImGuiCol_ResizeGripHovered]    = mauve;
		colors[ImGuiCol_ResizeGripActive]     = mauve;
		colors[ImGuiCol_Tab]                  = surface0;
		colors[ImGuiCol_TabHovered]           = surface2;
		colors[ImGuiCol_TabActive]            = surface1;
		colors[ImGuiCol_TabUnfocused]         = surface0;
		colors[ImGuiCol_TabUnfocusedActive]   = surface1;
		colors[ImGuiCol_DockingPreview]       = sapphire;
		colors[ImGuiCol_DockingEmptyBg]       = base;
		colors[ImGuiCol_PlotLines]            = blue;
		colors[ImGuiCol_PlotLinesHovered]     = peach;
		colors[ImGuiCol_PlotHistogram]        = teal;
		colors[ImGuiCol_PlotHistogramHovered] = green;
		colors[ImGuiCol_TableHeaderBg]        = surface0;
		colors[ImGuiCol_TableBorderStrong]    = surface1;
		colors[ImGuiCol_TableBorderLight]     = surface0;
		colors[ImGuiCol_TableRowBg]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_TableRowBgAlt]        = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
		colors[ImGuiCol_TextSelectedBg]       = surface2;
		colors[ImGuiCol_DragDropTarget]       = yellow;
		colors[ImGuiCol_NavHighlight]         = lavender;
		colors[ImGuiCol_NavWindowingHighlight]= ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
		colors[ImGuiCol_NavWindowingDimBg]    = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
		colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);
		colors[ImGuiCol_Text]                 = text;
		colors[ImGuiCol_TextDisabled]         = subtext0;

		// Rounded corners
		style.WindowRounding    = 6.0f;
		style.ChildRounding     = 6.0f;
		style.FrameRounding     = 4.0f;
		style.PopupRounding     = 4.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabRounding      = 4.0f;
		style.TabRounding       = 4.0f;

		// Padding and spacing
		style.WindowPadding     = ImVec2(8.0f, 8.0f);
		style.FramePadding      = ImVec2(5.0f, 3.0f);
		style.ItemSpacing       = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);
		style.IndentSpacing     = 21.0f;
		style.ScrollbarSize     = 14.0f;
		style.GrabMinSize       = 10.0f;

		// Borders
		style.WindowBorderSize  = 1.0f;
		style.ChildBorderSize   = 1.0f;
		style.PopupBorderSize   = 1.0f;
		style.FrameBorderSize   = 0.0f;
		style.TabBorderSize     = 0.0f;
	}
#endif
}
void SceneTree::draw_editors() {
#ifdef _DEBUG
	if (ImGui::Begin("Scene Graph")) {
		if (ImGui::BeginTable("SceneGraphTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::BeginChild("Entities", ImVec2(0,ImGui::GetContentRegionAvail().y), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove)) {
				ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 20.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 6.0f));
				editor_draw_entity_hierarchy_recursive(get_entity(root_id_));
				ImGui::PopStyleVar(3);
				ImGui::EndChild();
			}
			ImGui::TableNextColumn();
			constexpr ImGuiChildFlags child_flags = ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize;
			constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove;
			if (ImGui::BeginChild("Inspector", ImGui::GetContentRegionAvail(), child_flags, window_flags)) {
				if (this->scene_graph_selected_entity_.valid()) {
					get_entity(this->scene_graph_selected_entity_)->editor();
				}
				else {
					ImGui::Text("Click an entity in the hierarchy to inspect it.");
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndTable();
	}
	ImGui::End();
#endif
}

SharedPtr<Window> SceneTree::get_window() const {
	return window_;
}

void SceneTree::setup_render_pass(RenderPassInfo const &info) {
}

void SceneTree::dispose() {
	for (const std::pair entity : entities_){
		printf("Disposing entity %s\n", entity.second->name_.c_str());
		if (!entity.second->is_destroyed_) {
			printf("Entity %s is not destroyed, forcing destroy\n", entity.second->name_.c_str());
			assert(destroy_entity(entity.first) == OK);
		}
}
	this->entities_.clear();
	this->window_ = nullptr; // dec ref
}
bool SceneTree::disposed() const {
	return !window_ || window_->disposed();
}