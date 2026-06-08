#pragma once

// Keep literally every single exposed function here, this is effectively a massive list of things across the whole engine.

#define HELIXAPI extern "C" __declspec(dllexport)
#include <cstdint>

// Generic Engine Related Calls
HELIXAPI int32_t engine_get_active_window(void **pWindow);
HELIXAPI int32_t engine_get_scene_tree(void **pSceneTree);

// SceneTree
HELIXAPI int32_t scenetree_get_entity(void *pSceneTree, uint32_t entity_id, void **pEntity);
HELIXAPI int32_t scenetree_root(void *pSceneTree, void **pRootEntity);
HELIXAPI int32_t scenetree_get_window(void *pSceneTree, void **pWindow);

typedef void(__stdcall *scenetree_entity_visitor_fn)(uint32_t entity_id, void **user_data);
HELIXAPI int32_t scenetree_visit_entities(void *pSceneTree, scenetree_entity_visitor_fn visitor, void **user_data);

typedef void(__stdcall *scenetree_entity_component_visitor_fn)(uint32_t entity_id, uint32_t component_id, void *user_data);
HELIXAPI int32_t scenetree_visit_component(void *pSceneTree, uint32_t component_id, void *user_data);

// Entity

#pragma region graphics

typedef struct {
	
	int32_t width, height;

	int32_t layer;
	int32_t levels;
	
	uint32_t pixel_format, pixel_type;
	uint32_t internal_format;

	uint32_t wrap, mag, min;

	uint32_t compare_mode, compare_func;

	bool generate_mipmaps;
	uint32_t anisotropic_scale;
	
} texture_create_info_t;

HELIXAPI int32_t texture_create(void **pTexture, texture_create_info_t const *pCreateInfo);
HELIXAPI int32_t texture_free(void *pTexture);

#pragma endregion
