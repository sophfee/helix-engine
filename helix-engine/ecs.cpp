#include "ecs.hpp"
#include <climits>
#include <glm/gtc/matrix_transform.hpp>
#ifdef _DEBUG
#include "imgui/imgui.h"
#endif

#include <glm/gtc/type_ptr.inl>

#include "gltf.h"
#include "mesh.hpp"

// CEntity

CEntity::CEntity(CSharedPtr<CSceneTree> const &p_sceneTree, COptional<CString> const &p_name, COptional<uid> const p_uniqueId) :
	scene_tree_(p_sceneTree),
	name_(p_name.value_or("?")),
	unique_id_(p_uniqueId.value_or(UINT32_MAX)),
	children_(0),
	components_(0) {
}

CEntity::CEntity() : name_("?") {
}

CEntity::~CEntity() {
	Error const err = scene_tree_.lock()->removeEntity(this->unique_id_);
	assert(err == OK);
}

CSharedPtr<CEntity> CEntity::parent() const {
	assert(!is_root_); //< Root has no parent.
	assert(!scene_tree_.expired());
	return scene_tree_.lock()->entity(parent_id_);
}

CSharedPtr<CEntity> CEntity::child(_STD size_t const idx) const {
	assert(!scene_tree_.expired());
	assert(idx < children_.size());
	uid const childUid = children_[idx];
	assert(childUid != UINT32_MAX);
	return scene_tree_.lock()->entity(childUid);
}
void CEntity::setParent(CSharedPtr<CEntity> const &p_entity) {
	assert(!scene_tree_.expired());
	p_entity->addChild(shared_from_this());
}

void CEntity::addChild(CSharedPtr<CEntity> const &p_entity) {
	assert(!scene_tree_.expired());
	children_.push_back(p_entity->id());
	if (p_entity->parent_id_ != UINT32_MAX)
		p_entity->parent()->removeChild(p_entity);
	p_entity->parent_id_ = unique_id_;
}

void CEntity::removeChild(CSharedPtr<CEntity> const &p_entity) {
	assert(!scene_tree_.expired());
	CSharedPtr<CEntity> parent = scene_tree_.lock()->entity(p_entity->parent_id_);
	children_.erase(_STD ranges::find(children_, p_entity->id()));
	p_entity->parent_id_ = UINT32_MAX;
}

_STD size_t CEntity::componentCount() const {
	return components_.size();
}

uid CEntity::id() const {
	return unique_id_;
}

CSharedPtr<CSceneTree> CEntity::tree() const {
	assert(!scene_tree_.expired());
	return scene_tree_.lock();
}

#ifdef _DEBUG

void CEntity::editor() {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	if (children_.empty() && components_.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	ImGui::PushStyleColor(ImGuiCol_Text, ImColor::HSV(.33f, .5f, .9f).Value);
	if (ImGui::TreeNodeEx(("[E] " + name_).c_str(), flags)) {
		ImGui::PopStyleColor(1);
		if (!components_.empty())
			for (Component *component : components_) {
				assert(component != nullptr);
				component->editor();
			}

		if (!children_.empty()) {
			
			for (auto id : children_) {
				CSharedPtr<CEntity> const child = scene_tree_.lock()->entity(id);
				child->editor();
			}
		}

		ImGui::TreePop();
	}
	else {
		ImGui::PopStyleColor(1);
	}
}

#endif

Component::Component(CWeakPtr<CSceneTree> const &p_scene_tree, CWeakPtr<CEntity> const &p_entity):
	tree(p_scene_tree), entity(p_entity) {
}
Component::~Component() = default;

void Component::init() {}
void Component::destroy() {
	CSharedPtr<CEntity> ent = entity.lock();
	CComponentServer<_STD remove_cvref_t<decltype(*this)>>::remove(ent);
}
void Component::wake() {}
void Component::sleep() {}
void Component::update(double) {
	//_STD cout << "Component::update" << '\n';
}
void Component::editor() {}

CComponentServer<Component> CComponentServer<Component>::instance_ = CComponentServer();
CComponentServer<Transform> CComponentServer<Transform>::instance_ = CComponentServer();
CComponentServer<Mesh> CComponentServer<Mesh>::instance_ = CComponentServer();

glm::mat4 Transform::matrix() const {
	CSharedPtr<CEntity> const parent = entity.lock()->parent();
	glm::mat4 mat = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation);
	mat = glm::scale(mat, scale);
	if (parent->hasComponent<Transform>()) {
		Transform const &parentTransform = parent->component<Transform>();
		mat = parentTransform.matrix() * mat; // recursively apply all matrices.
	}
	return mat;
}

#ifdef _DEBUG
void Transform::editor() {
	if (ImGui::TreeNode("Transform")) {
		ImGui::InputFloat3("Translation", &translation[0]);
		ImGui::InputFloat4("Quaternion", &rotation[0]);
		ImGui::InputFloat3("Scale", &scale[0]);
		ImGui::TreePop();
	}
}
#endif


void Mesh::update(double x) {
	_STD cout << "Mesh::update" << '\n';
	mesh->drawAllSubMeshes();
}

#ifdef _DEBUG
void Mesh::editor() {
	if (ImGui::TreeNodeEx("[C] Mesh")) {
		ImGui::Text("%llu primitives", mesh->primitives_.size());
		ImGui::TreePop();
	}
}
#endif

//
// CSceneTree
//



CSceneTree::CSceneTree() {
	//< nothing as of now.
}
CSceneTree::~CSceneTree() {
	//< Nothing as of now.
}

CResult<uid> CSceneTree::createEntity() {
	if (!empty_slots_.empty())
		return createEntityFromVacantAllocatedSlot_();

	// Check allocated space
	if (entities_.size() == entities_.capacity())
		entities_.reserve(entities_.capacity() + 128); // allocate in big chunks because doing this often sucks ass.
	entities_.push_back(_STD make_shared<CEntity>( shared_from_this(), _STD nullopt, entities_.size()));
	return entities_.back()->unique_id_;
}

Error CSceneTree::removeEntity(uid p_uid) {
	// Get entity there
	CSharedPtr<CEntity> ent = entities_.at(p_uid);
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
void CSceneTree::setRoot(uid const uid) {
	if (root_id_ != UINT32_MAX)
		entities_[root_id_]->is_root_ = false;
	entities_[uid]->is_root_ = true;
	root_id_ = uid;
}

CSharedPtr<CEntity> CSceneTree::entity(uid const idx) {
	assert(idx < entities_.size());
	return entities_[idx];
}

CVector<CSharedPtr<CEntity>> const & CSceneTree::entities() const {
	assert(!entities_.empty());
	return entities_;
}

void CSceneTree::initiateFrame() {
	_STD cout << ">>CSceneTree::initiateFrame()\n";
	frame(root_id_);
	_STD cout << "<<CSceneTree::initiateFrame()\n";
}

void CSceneTree::drawEditors() const {
	entities_[root_id_]->editor();
}

void CSceneTree::frame(uid const on) {
	CSharedPtr<CEntity> const ent = entities_.at(on);
	
	for (Component *c : ent->components_) {
		c->update(0.0);
	}
	for (uid const child : ent->children_)
		frame(child); // recursive down the scene tree.
}

CResult<uid> CSceneTree::createEntityFromVacantAllocatedSlot_() {
	uid const ent_id = empty_slots_.front();
	empty_slots_.pop();
	CSharedPtr<CEntity> const ent = entities_.at(ent_id);
	ent->name_ = "name";
	ent->unique_id_ = ent_id;
	return ent_id;
}

namespace {
	uid node2entity(GltfData_t &gltf_data, CSharedPtr<CSceneTree> const &tree, GltfNode_t &node, uid node_id, _STD vector<uid> &node_id_to_entity_id) {
		uid const ent_id = tree->createEntity();
		node_id_to_entity_id[node_id] = ent_id;
		CSharedPtr<CEntity> const ent = tree->entity(ent_id);
		ent->name_ = node.name;

		if (node.has_transform) {
			Transform &xform = ent->component<Transform>();
			xform.translation = node.translation;
			xform.rotation = node.rotation;
			xform.scale = node.scale;
		}

		if (node.mesh != -1) {
			Mesh &mesh_component = ent->component<Mesh>();
			if (node.skin != -1)
				mesh_component.mesh.reset(new CMesh(gltf_data, node.mesh));
			else
				mesh_component.mesh.reset(new CMesh(gltf_data, node.mesh, node.skin));
		}

		for (gltf::id const child : node.children) {
			uid const child_id = node2entity(gltf_data, tree, gltf_data.nodes[child], child, node_id_to_entity_id);
			ent->addChild(tree->entity(child_id));
		}
            
		return ent_id;
	}
	
}

uid gltf::createEntityFromGltf(CSharedPtr<CSceneTree> const &scene_tree, GltfData_t &data) {
	_STD vector<uid> node_id_to_entity_id(data.nodes.size());
	uid const true_root = scene_tree->createEntity().value(); //< So because there can be multiple top level nodes in gltf, we have one entity residing as the top-level
	CSharedPtr<CEntity> scene = scene_tree->entity(true_root);
	scene->name_ = data.scenes[data.scene].name;
	
	for (uid const node_id : data.scenes[data.scene].nodes) {
		uid const node = node2entity(data, scene_tree, data.nodes[node_id], node_id, node_id_to_entity_id);
		scene->addChild(scene_tree->entity(node));
	}
	
	return true_root;
}
