#include "ecs_gltf.hpp"
#include "mesh-renderer.h"
#include "transform.h"
#include "light.hpp"

namespace gltf {
	RID node2entity(gltf::Data &gltf_data, Vector<SharedPtr<Buffer>> &buffer_views, SharedPtr<SceneTree> const &tree, gltf::Node &node, RID node_id, _STD vector<RID> &node_id_to_entity_id) {
		RID const ent_id = tree->create_entity();
		node_id_to_entity_id[node_id] = ent_id;
		Entity* ent = tree->get_entity(ent_id);
		ent->name_ = node.name;

		if (node.has_transform) {
			Transform &xform = ent->get_component<Transform>();
			xform.translation = node.translation;
			xform.rotation = node.rotation;
			xform.scale = glm::length(node.scale) < 0.001f ? vec3_one : node.scale;
		}

		if (node.mesh != -1) {
			StaticMeshRenderer3D &mesh_component = ent->get_component<StaticMeshRenderer3D>();
			mesh_component.mesh.reset(new ::Mesh(gltf_data, node.mesh, buffer_views));
		}

		if (node.extensions.KHR_lights_punctual.has_value()) {
			OmniLight &light = ent->get_component<OmniLight>();
			auto const [name, color, intensity, type, range, spot] = gltf_data.extensions.KHR_lights_punctual.value().lights[node.extensions.KHR_lights_punctual.value().light];
			light.set_enabled(true);
			light.set_position(node.translation);
			light.set_intensity(intensity);
			light.set_color(color);
			light.set_range(range);
		}

		for (gltf::id const child : node.children) {
			RID const child_id = node2entity(gltf_data, buffer_views, tree, gltf_data.nodes[child], child, node_id_to_entity_id);
			ent = tree->get_entity(ent_id); // RIDs are persistent, but a pointer is NOT!!!
			ent->add_child(tree->get_entity(child_id));
		}
            
		return ent_id;
	}
}

RID gltf::create_entity_from_gltf(SharedPtr<SceneTree> const &scene_tree, Data &data) {
	_STD vector<RID> node_id_to_entity_id(data.nodes.size());
	RID const true_root = scene_tree->create_entity().value(); //< So because there can be multiple top level nodes in gltf, we have one entity residing as the top-level
	Entity* scene = scene_tree->get_entity(true_root);
	scene->name_ = data.scenes[data.scene].name;
	scene->scene_tree_ = scene_tree;
	
	Vector<SharedPtr<Buffer>> buffer_views(data.buffer_views.size()); // These will get allocated as needed by the mesh importer
	
	for (gltf::id const node_id : data.scenes[data.scene].nodes) {
		RID const node = node2entity(data, buffer_views, scene_tree, data.nodes[node_id], node_id, node_id_to_entity_id);
		scene = scene_tree->get_entity(true_root); // RIDs are persistent, but a pointer is NOT!!!
		scene->add_child(scene_tree->get_entity(node));
	}

#ifdef GLTF_SKIN
	for (RID const node_id : data.scenes[data.scene].nodes) {
		parseNodeBoneMap(data, scene_tree, scene_tree->get_entity(node_id_to_entity_id[node_id]), data.nodes[node_id], node_id_to_entity_id); 
	}
#endif
	
	return true_root;
}
