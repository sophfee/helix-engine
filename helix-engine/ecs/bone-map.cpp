#include "bone-map.h"

#include "imgui.h"
#include "transform.h"

ComponentServer<BoneMap> ComponentServer<BoneMap>::instance_ = ComponentServer();

BoneMap::BoneMap(Weak<SceneTree> const &p_scene_tree, Weak<Entity> const &p_entity): Component(p_scene_tree, p_entity), bone_map_buffer_(_STD make_shared<Buffer>()) {}

void BoneMap::addBoneMapping(uid const entity_id, _STD size_t const mapping_value) {
	if (mapping_value > bone_map_.size())
		bone_map_.resize(mapping_value + 1);
	bone_map_[mapping_value] = entity_id;
}

SharedPtr<Entity> BoneMap::boneMappedEntity(_STD size_t const index) const {
	SharedPtr<Entity> owner = this->entity.lock();
	return owner->tree()->entity(bone_map_[index]);
}

SharedPtr<Entity> BoneMap::operator[](_STD size_t const index) const {
	return boneMappedEntity(index);
}

void BoneMap::updateBuffer() const {
	bone_map_buffer_->invalidateData();

	_STD vector<glm::mat4> bone_buffer_data(0);

	SharedPtr<Entity> const owner = this->entity.lock();
	SharedPtr<SceneTree> const tree = owner->tree();
	for (uid const ent_id : bone_map_) {
		SharedPtr<Entity> const bone_entity = tree->entity(ent_id);
		assert(bone_entity->hasComponent<Transform>());
		Transform &transform = bone_entity->component<Transform>();
		if (glm::length(transform.scale) == 0.0f) {
			transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		auto matrix = transform.matrix();
		/*
		printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
			matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3], matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
			matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3], matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
			*/
		bone_buffer_data.push_back(matrix);
	}

	bone_map_buffer_->allocStorage(bone_buffer_data.size() * sizeof(glm::mat4), bone_buffer_data.data(), gl::BufferStorageMask::DynamicStorageBit);
	bone_map_buffer_->upload(bone_buffer_data.size() * sizeof(glm::mat4), bone_buffer_data.data(), gl::BufferUsageARB::StaticDraw);
	gpu_check;
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void BoneMap::bindBuffer() const {
	bone_map_buffer_->bindBufferBase(gl::BufferTargetARB::ShaderStorageBuffer, 0);
	inverse_bind_buffer_->bindBufferBase(gl::BufferTargetARB::ShaderStorageBuffer, 1);
}
void BoneMap::editor() {
	if (ImGui::TreeNode("Skeleton")) {
		_STD size_t const bone_map_size = bone_map_.size(); 
		ImGui::Text("%llu", bone_map_size);

		ImGui::TreePop();
	}
}

