#pragma once

#include "ecs.hpp"

struct TransformMatrices_t {
	glm::mat4 translate;
	glm::mat4 rotation;
	glm::mat4 scale;
};

struct transform_data {
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
};

class CTransform : public Component {
public:
	CTransform(CSharedPtr<CSceneTree> const &p_tree, CSharedPtr<CEntity> const &p_entity);

	glm::vec3 translation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::quat rotation = glm::quat();

	// Only computes local space translation!
	_NODISCARD glm::mat4 computeTranslation() const;

	_NODISCARD glm::vec3 position() const;
	
	// Only computes local space rotation!
	_NODISCARD glm::mat4 computeRotation() const;

	_NODISCARD glm::quat orientation() const;
	
	// Only computes local space scale!
	_NODISCARD glm::mat4 computeScale() const;
	// Performs hierarchy calculated matrix transformation
	_NODISCARD TransformMatrices_t computeTransformMatrices() const;
	
	[[nodiscard]] glm::mat4 matrix() const;

#ifdef _DEBUG
	void editor() override;
#endif
};
