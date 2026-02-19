#include "ecs.hpp"
#include <climits>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"

// CEntity

CEntity::CEntity(CSceneTree *p_sceneTree, COptional<CString> const &p_name, COptional<uid> const p_uniqueId)
	: scene_tree_(p_sceneTree), name_(p_name.value_or("?")), unique_id_(p_uniqueId.value_or(UINT32_MAX)) {
}

CEntity::CEntity() : scene_tree_(nullptr), name_("?") {
}

CEntity::~CEntity() {
	Error const err = scene_tree_->removeEntity(this->unique_id_);
	assert(err == OK);
}
CEntity const & CEntity::parent() const {
	assert(!is_root_); //< Root has no parent.
	return scene_tree_->entity(parent_id_);
}
CEntity & CEntity::parent() {
	assert(!is_root_);
	return const_cast<CEntity &>(scene_tree_->entity(parent_id_));
}

CEntity const& CEntity::child(std::size_t const idx) const {
	assert(idx < children_.size());
	uid const childUid = children_[idx];
	assert(childUid != UINT32_MAX);
	return scene_tree_->entity(childUid);
}
void CEntity::setParent(CEntity &p_entity) {
	p_entity.addChild(*this);
}

void CEntity::addChild(CEntity &p_entity) {
	children_.push_back(p_entity.id());
	if (p_entity.parent_id_ != UINT32_MAX)
		p_entity.parent().removeChild(p_entity);
	p_entity.parent_id_ = unique_id_;
}

void CEntity::removeChild(CEntity &p_entity) {
	auto &parent = scene_tree_->entity(p_entity.parent_id_);
	children_.erase(std::ranges::find(children_, p_entity.id()));
	p_entity.parent_id_ = UINT32_MAX;
}

uid CEntity::id() const {
	return unique_id_;
}

CSceneTree * CEntity::tree() const {
	return scene_tree_;
}

Component::Component(CSceneTree *tree, CEntity *ent) : tree(tree), entity(ent) {
}
Component::~Component() {}

void Component::init() {}
void Component::destroy() {
	CEntity const &ent = *entity;
	CComponentServer<std::remove_cvref_t<decltype(*this)>>::remove(ent);
}
void Component::wake() {}
void Component::sleep() {}
void Component::update(double) {}

CComponentServer<Component> CComponentServer<Component>::instance_ = CComponentServer();
CComponentServer<Transform> CComponentServer<Transform>::instance_ = CComponentServer();
CComponentServer<Mesh> CComponentServer<Mesh>::instance_ = CComponentServer();

glm::mat4 Transform::matrix() const {
	auto &parent = const_cast<CEntity &>(entity->parent());
	
	glm::mat4 mat = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation);
	mat = glm::scale(mat, scale);
	if (parent.hasComponent<Transform>()) {
		Transform const &parentTransform = parent.component<Transform>();
		mat = parentTransform.matrix() * mat; // recursively apply all matrices.
	}
	return mat;
}

Mesh::Mesh(CSceneTree *p_tree, CEntity *p_ent): Component(p_tree, p_ent), mesh(std::make_unique<CMesh>()) {
}

void Mesh::update(double x) {
	mesh->drawAllSubMeshes();
}

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
	entities_.push_back(std::make_shared<CEntity>(this, std::nullopt, entities_.size()));
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

CEntity & CSceneTree::entity(uid const idx) {
	assert(idx < entities_.size());
	return *entities_[idx];
}

CVector<CSharedPtr<CEntity>> const & CSceneTree::entities() const {
	assert(!entities_.empty());
	return entities_;
}

void CSceneTree::initiateFrame() {
	frame(root_id_);
}

void CSceneTree::frame(uid on) {
	CSharedPtr<CEntity> const ent = entities_.at(on);
	for (Component *c : ent->components_)
		c->update(0.0);
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
