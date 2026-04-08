#include "ecs_gltf.hpp"
#include "mesh-renderer.h"
#include "transform.h"
#include "bone-map.h"

namespace gltf {
	uid node2entity(gltf::data &gltf_data, SharedPtr<SceneTree> const &tree, gltf::node &node, uid node_id, _STD vector<uid> &node_id_to_entity_id) {
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
#ifdef GLTF_SKIN
			if (node.skin != -1) {
				mesh_component.mesh.reset(new Mesh(gltf_data, node.mesh, node.skin));
				// We need a post-hook to obtain the final entity id's for each joint!
				auto &b = ent->component<BoneMap>(); // we are just instantiating it here.
				b.skin = node.skin;
			}
			else
#endif
				mesh_component.mesh.reset(new Mesh(gltf_data, node.mesh));
		}

		if (node.extensions.KHR_lights_punctual.has_value()) {
			/*
			OmniLight &light = ent->component<OmniLight>();
			auto const [name, color, intensity, type, range, spot] = gltf_data.extensions.KHR_lights_punctual.value().lights[node.extensions.KHR_lights_punctual.value().light];
			light.setPosition(node.translation);
			light.setIntensity(intensity);
			light.setColor(color);
			light.setRange(range);
		*/}

		for (gltf::id const child : node.children) {
			uid const child_id = node2entity(gltf_data, tree, gltf_data.nodes[child], child, node_id_to_entity_id);
			ent->addChild(tree->entity(child_id));
		}
            
		return ent_id;
	}

	void parseNodeBoneMap(gltf::data &gltf_data, SharedPtr<SceneTree> const &tree, SharedPtr<Entity> me, gltf::node &node, _STD vector<uid> &node_id_to_entity_id) {
		if (me->hasComponent<BoneMap>() && me->hasComponent<StaticMeshRenderer3D>()) {
			BoneMap &bone_map = me->component<BoneMap>();
			for (gltf::id const joint : gltf_data.skins[bone_map.skin].joints)
				bone_map.addBoneMapping(node_id_to_entity_id[joint], joint);
			bone_map.updateBuffer();
			
			auto &bv = gltf_data.buffer_views[gltf_data.accessors[gltf_data.skins[bone_map.skin].inverseBindMatrices].bufferView()];
			bone_map.inverse_bind_buffer_.reset(new Buffer);
			bone_map.inverse_bind_buffer_->allocStorage(bv.length, &gltf_data.buffers[0].data()[bv.offset], gl::BufferStorageMask::DynamicStorageBit);
			bone_map.inverse_bind_buffer_->upload(bv.length, &gltf_data.buffers[0].data()[bv.offset], gl::BufferUsageARB::StaticDraw);
		}
#ifdef GLTF_SKIN
		for (uid const child : me->children_) {
			parseNodeBoneMap(gltf_data, tree, tree->entity(child), node, node_id_to_entity_id);
		}
#endif
	}
}

uid gltf::createEntityFromGltf(SharedPtr<SceneTree> const &scene_tree, data &data) {
	_STD vector<uid> node_id_to_entity_id(data.nodes.size());
	uid const true_root = scene_tree->createEntity().value(); //< So because there can be multiple top level nodes in gltf, we have one entity residing as the top-level
	SharedPtr<Entity> scene = scene_tree->entity(true_root);
	scene->name_ = data.scenes[data.scene].name;
	
	for (uid const node_id : data.scenes[data.scene].nodes) {
		uid const node = node2entity(data, scene_tree, data.nodes[node_id], node_id, node_id_to_entity_id);
		scene->addChild(scene_tree->entity(node));
	}

#ifdef GLTF_SKIN
	for (uid const node_id : data.scenes[data.scene].nodes) {
		parseNodeBoneMap(data, scene_tree, scene_tree->entity(node_id_to_entity_id[node_id]), data.nodes[node_id], node_id_to_entity_id); 
	}
#endif
	
	return true_root;
}
