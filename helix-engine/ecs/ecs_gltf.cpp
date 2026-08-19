#include "ecs_gltf.hpp"
#include "mesh-renderer.h"
#include "transform.h"
#include "light.hpp"

namespace gltf {
	uid node2entity(gltf::data &gltf_data, Vector<SharedPtr<Buffer>> &buffer_views, SharedPtr<SceneTree> const &tree, gltf::node &node, uid node_id, _STD vector<uid> &node_id_to_entity_id) {
		uid const ent_id = tree->createEntity();
		node_id_to_entity_id[node_id] = ent_id;
		SharedPtr<Entity> const ent = tree->entity(ent_id);
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
			uid const child_id = node2entity(gltf_data, buffer_views, tree, gltf_data.nodes[child], child, node_id_to_entity_id);
			ent->addChild(tree->entity(child_id));
		}
            
		return ent_id;
	}
}

uid gltf::createEntityFromGltf(SharedPtr<SceneTree> const &scene_tree, data &data) {
	_STD vector<uid> node_id_to_entity_id(data.nodes.size());
	uid const true_root = scene_tree->createEntity().value(); //< So because there can be multiple top level nodes in gltf, we have one entity residing as the top-level
	SharedPtr<Entity> scene = scene_tree->entity(true_root);
	scene->name_ = data.scenes[data.scene].name;
	
	Vector<SharedPtr<Buffer>> buffer_views(data.buffer_views.size()); // These will get allocated as needed by the mesh importer
	
	for (uid const node_id : data.scenes[data.scene].nodes) {
		uid const node = node2entity(data, buffer_views, scene_tree, data.nodes[node_id], node_id, node_id_to_entity_id);
		scene->addChild(scene_tree->entity(node));
	}

#ifdef GLTF_SKIN
	for (uid const node_id : data.scenes[data.scene].nodes) {
		parseNodeBoneMap(data, scene_tree, scene_tree->entity(node_id_to_entity_id[node_id]), data.nodes[node_id], node_id_to_entity_id); 
	}
#endif
	
	return true_root;
}
