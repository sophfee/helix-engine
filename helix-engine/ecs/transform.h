#pragma once

#include "ecs.hpp"

class Transform : public Component {
public:
	Transform(CSharedPtr<CSceneTree> const &p_tree, CSharedPtr<CEntity> const &p_entity);

	glm::vec3 translation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::quat rotation = glm::quat();
	
	[[nodiscard]] glm::mat4 matrix() const;

#ifdef _DEBUG
	void editor() override;
#endif
};
