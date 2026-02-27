#pragma once
#include "ecs.hpp"

class BoneMap : public Component {
public:
	BoneMap(CWeakPtr<CSceneTree> const &p_scene_tree, CWeakPtr<CEntity> const &p_entity);

	void addBoneMapping(uid entity_id, _STD size_t mapping_value);

	_NODISCARD CSharedPtr<CEntity> boneMappedEntity(_STD size_t index) const;
	_NODISCARD CSharedPtr<CEntity> operator[](_STD size_t index) const;

	void updateBuffer() const;
	void bindBuffer() const;

	_STD size_t skin;
	_STD vector<uid> bone_map_;
	
	CSharedPtr<CBuffer> inverse_bind_buffer_;
	CSharedPtr<CBuffer> bone_map_buffer_;

	virtual void editor() override;
private:
};
