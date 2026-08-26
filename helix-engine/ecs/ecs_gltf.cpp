#include "ecs_gltf.hpp"
#include "mesh-renderer.h"
#include "transform.h"
#include "light.hpp"

namespace gltf {
	RID node2entity(gltf::data &gltf_data, Vector<SharedPtr<Buffer>> &buffer_views, SharedPtr<SceneTree> const &tree, gltf::node &node, RID node_id, _STD vector<RID> &node_id_to_entity_id) {
		RID const ent_id = tree->createEntity();
		node_id_to_entity_id[node_id] = ent_id;
		Entity* ent = tree->entityMut(ent_id);
		ent->name_ = node.name;

		if (node.has_transform) {
			Transform &xform = ent->component<Transform>();
			xform.translation = node.translation;
			xform.rotation = node.rotation;
			xform.scale = glm::length(node.scale) < 0.001f ? vec3_one : node.scale;
			
		}

		if (node.mesh != -1) {
			StaticMeshRenderer3D &mesh_component = ent->component<StaticMeshRenderer3D>();
			mesh_component.mesh.reset(new Mesh(gltf_data, node.mesh, buffer_views));
		}

		if (node.extensions.KHR_lights_punctual.has_value()) {
			OmniLight &light = ent->component<OmniLight>();
			auto const [name, color, intensity, type, range, spot] = gltf_data.extensions.KHR_lights_punctual.value().lights[node.extensions.KHR_lights_punctual.value().light];
			light.setEnabled(true);
			light.setPosition(node.translation);
			light.setIntensity(intensity);
			light.setColor(color);
			light.setRange(range);
		}

		for (gltf::id const child : node.children) {
			RID const child_id = node2entity(gltf_data, buffer_views, tree, gltf_data.nodes[child], child, node_id_to_entity_id);
			ent = tree->entityMut(ent_id); // RIDs are persistent, but a pointer is NOT!!!
			ent->addChild(tree->entityMut(child_id));
		}
            
		return ent_id;
	}
}

RID gltf::createEntityFromGltf(SharedPtr<SceneTree> const &scene_tree, data &data) {
	_STD vector<RID> node_id_to_entity_id(data.nodes.size());
	RID const true_root = scene_tree->createEntity().value(); //< So because there can be multiple top level nodes in gltf, we have one entity residing as the top-level
	Entity* scene = scene_tree->entityMut(true_root);
	scene->name_ = data.scenes[data.scene].name;
	scene->scene_tree_ = scene_tree;
	
	Vector<SharedPtr<Buffer>> buffer_views(data.buffer_views.size()); // These will get allocated as needed by the mesh importer
	
	for (gltf::id const node_id : data.scenes[data.scene].nodes) {
		RID const node = node2entity(data, buffer_views, scene_tree, data.nodes[node_id], node_id, node_id_to_entity_id);
		scene = scene_tree->entityMut(true_root); // RIDs are persistent, but a pointer is NOT!!!
		scene->addChild(scene_tree->entityMut(node));
	}

#ifdef GLTF_SKIN
	for (RID const node_id : data.scenes[data.scene].nodes) {
		parseNodeBoneMap(data, scene_tree, scene_tree->entityMut(node_id_to_entity_id[node_id]), data.nodes[node_id], node_id_to_entity_id); 
	}
#endif
	
	return true_root;
}
