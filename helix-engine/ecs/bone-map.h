#pragma once
#include "ecs.hpp"

class BoneMap : public Component {
public:
	BoneMap(Weak<SceneTree> const &p_scene_tree, Weak<Entity> const &p_entity);

	void addBoneMapping(uid entity_id, _STD size_t mapping_value);

	_NODISCARD SharedPtr<Entity> boneMappedEntity(_STD size_t index) const;
	_NODISCARD SharedPtr<Entity> operator[](_STD size_t index) const;

	void updateBuffer() const;
	void bindBuffer() const;

	_STD size_t skin;
	_STD vector<uid> bone_map_;
	
	SharedPtr<Buffer> inverse_bind_buffer_;
	SharedPtr<Buffer> bone_map_buffer_;

	virtual void editor() override;
private:
};
