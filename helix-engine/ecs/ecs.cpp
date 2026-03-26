#include "ecs.hpp"
#include <climits>
#include <glm/gtc/matrix_transform.hpp>


#include <glm/gtc/type_ptr.inl>

#include "gpu/gltf.h"
#include "gpu/mesh.hpp"
#include "imgui.h"
#include "light.hpp"
#include "util.hpp"
#include "ecs/bone-map.h"
#include "ecs/mesh-renderer.h"
#include "ecs/transform.h"

// Entity

Entity::Entity(SharedPtr<SceneTree> const &p_sceneTree, Optional<String> const &p_name, Optional<uid> const p_uniqueId) :
	scene_tree_(p_sceneTree),
	name_(p_name.value_or("?")),
	unique_id_(p_uniqueId.value_or(UINT32_MAX)),
	children_(0),
	components_(0) {
}

Entity::Entity() : name_("?") {
}

Entity::~Entity() {
	if (scene_tree_.expired()) return;
	Error const err = scene_tree_.lock()->removeEntity(this->unique_id_);
	assert(err == OK);
}

SharedPtr<Entity> Entity::parent() const {
	assert(!is_root_); //< Root has no parent.
	assert(!scene_tree_.expired());
	return scene_tree_.lock()->entity(parent_id_);
}

SharedPtr<Entity> Entity::child(_STD size_t const idx) const {
	assert(!scene_tree_.expired());
	assert(idx < children_.size());
	uid const childUid = children_[idx];
	assert(childUid != UINT32_MAX);
	return scene_tree_.lock()->entity(childUid);
}
Vec<SharedPtr<Entity>> Entity::children() const {
	Vec<SharedPtr<Entity>> result(children_.size());
	for (uid const child : children_)
		result.push_back(scene_tree_.lock()->entity(child));
	return result;
}

bool Entity::root() const {
	return is_root_;
}

void Entity::setParent(SharedPtr<Entity> const &entity) {
	assert(!scene_tree_.expired());
	entity->addChild(shared_from_this());
}

void Entity::addChild(SharedPtr<Entity> const &entity) {
	assert(!scene_tree_.expired());
	children_.push_back(entity->id());
	if (entity->parent_id_ != UINT32_MAX)
		entity->parent()->removeChild(entity);
	entity->parent_id_ = unique_id_;
}

void Entity::removeChild(SharedPtr<Entity> const &entity) {
	assert(!scene_tree_.expired());
	SharedPtr<Entity> parent = scene_tree_.lock()->entity(entity->parent_id_);
	children_.erase(_STD ranges::find(children_, entity->id()));
	entity->parent_id_ = UINT32_MAX;
}

_STD size_t Entity::componentCount() const {
	return components_.size();
}

uid Entity::id() const {
	return unique_id_;
}

SharedPtr<SceneTree> Entity::tree() const {
	assert(!scene_tree_.expired());
	return scene_tree_.lock();
}

SharedPtr<Window> Entity::window() const {
	return scene_tree_.lock()->window();
}

#ifdef _DEBUG

void Entity::editor() {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	if (children_.empty() && components_.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(.33f, .5f, .9f).Value);
	auto const tree_node_id = std::vformat("{} ({})", std::make_format_args(name_, unique_id_));
	if (ImGui::TreeNodeEx(tree_node_id.c_str(), flags)) {
		ImGui::PopStyleColor(1);
		if (!components_.empty())
			for (Component *component : components_) {
				assert(component != nullptr);
				component->editor();
			}

		if (!children_.empty()) {
			
			for (auto id : children_) {
				SharedPtr<Entity> const child = scene_tree_.lock()->entity(id);
				child->editor();
			}
		}

		ImGui::TreePop();
	}
	else {
		ImGui::PopStyleColor(1);
	}
	debug_hovered_ = ImGui::IsItemHovered();
}

#endif

Component::Component(Weak<SceneTree> const &scene_tree, Weak<Entity> const &entity):
	tree(scene_tree), entity(entity) {
}
Component::~Component() = default;

void Component::init() {}
void Component::destroy() {
	SharedPtr<Entity> ent = entity.lock();
	ComponentServer<_STD remove_cvref_t<decltype(*this)>>::remove(ent);
}
void Component::wake() {}
void Component::sleep() {}
void Component::update(double) {
	//_STD cout << "Component::update" << '\n';
}
void Component::renderSetup(RenderPassInfo const &info) {}
void Component::draw(RenderPassInfo const &info) {}
void Component::editor() {}

SharedPtr<Window> Component::window() const {
	return entity.lock()->window();
}

ComponentServer<Component> ComponentServer<Component>::instance_ = ComponentServer();

//
// SceneTree
//



SceneTree::SceneTree(SharedPtr<Window> const &window)
	: window_(window) {
}

SceneTree::~SceneTree() {
	//< Nothing as of now.
}

Result<uid> SceneTree::createEntity() {
	if (!empty_slots_.empty())
		return createEntityFromVacantAllocatedSlot_();

	// Check allocated space
	if (entities_.size() == entities_.capacity())
		entities_.reserve(entities_.capacity() + 128); // allocate in big chunks because doing this often sucks ass.
	entities_.push_back(_STD make_shared<Entity>( shared_from_this(), _STD nullopt, entities_.size()));
	return entities_.back()->unique_id_;
}

Error SceneTree::removeEntity(uid id) {
	// Get entity there
	if (id >= entities_.size())
		return ERR_OUT_OF_RANGE;
	
	SharedPtr<Entity> const ent = entities_.at(id);
	ent->is_enabled_ = false;
	ent->is_destroyed_ = true;
	
	Error err = OK;
	for (uid const cid : ent->children_) {
		err = removeEntity(cid); // Recursive freeing.
		assert(err == OK);
	}

	ent->children_.clear();
	ent->children_.shrink_to_fit();

	for (Component *c : ent->components_) {
		c->destroy();
	}

	ent->components_.clear();
	ent->components_.shrink_to_fit();

	// Remove myself from my parent
	if (!ent->is_root_) {
		err = removeEntity(ent->parent_id_);
		assert(err == OK);
	}

	ent->name_ = "deleted ent";
	
	empty_slots_.push(ent->id());
	return OK;
}
void SceneTree::setRoot(uid const uid) {
	if (root_id_ != UINT32_MAX)
		entities_[root_id_]->is_root_ = false;
	entities_[uid]->is_root_ = true;
	root_id_ = uid;
}

SharedPtr<Entity> SceneTree::entity(uid const idx) {
	assert(idx < entities_.size());
	return entities_[idx];
}

Vec<SharedPtr<Entity>> const & SceneTree::entities() const {
	assert(!entities_.empty());
	return entities_;
}

void SceneTree::initiateFrame() {
	//_STD cout << ">>SceneTree::initiateFrame()\n";
	frame(root_id_);
	//_STD cout << "<<SceneTree::initiateFrame()\n";
}
void SceneTree::initiateDraw(RenderPassInfo const &info) {
	draw(root_id_, info);
}

void SceneTree::initiateRenderSetup(RenderPassInfo const &info) {
	renderSetup(root_id_, info);
}

void SceneTree::drawEditors() const {
	entities_[root_id_]->editor();
}

SharedPtr<Window> SceneTree::window() const {
	return window_;
}

void SceneTree::frame(uid const on) {
	SharedPtr<Entity> const ent = entities_.at(on);
	
	for (Component *c : ent->components_)
		c->update(0.0);
	
	for (uid const child : ent->children_) {
		if (startsWith(ent->name_, "decal"))
			continue; // these are making me mad as they occlude the view of normals.
		frame(child); // recursive down the scene tree.
	}
}

void SceneTree::renderSetup(uid const on, RenderPassInfo const &info) {
	SharedPtr<Entity> const ent = entities_.at(on);
	
	for (Component *c : ent->components_)
		c->renderSetup(info);
	
	for (uid const child : ent->children_)
		renderSetup(child, info); // recursive down the scene tree.
}

void SceneTree::draw(uid const on, RenderPassInfo const &info) {
	SharedPtr<Entity> const ent = entities_.at(on);
	
	for (Component *c : ent->components_)
		c->draw(info);
	
	for (uid const child : ent->children_)
		draw(child, info); // recursive down the scene tree.
}

Result<uid> SceneTree::createEntityFromVacantAllocatedSlot_() {
	uid const ent_id = empty_slots_.front();
	empty_slots_.pop();
	SharedPtr<Entity> const ent = entities_.at(ent_id);
	ent->name_ = "name";
	ent->unique_id_ = ent_id;
	return ent_id;
}

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
