#pragma once

#include "asset.hpp"

class SceneTree;
class EntityRef;

class SceneAsset : public Asset {
public:
	using SelfType = SceneAsset;

	virtual void instantiate(const SharedPtr<SceneTree>& scene_tree, EntityRef& entity_ref) const;	
	
	[[nodiscard]] StringView get_class_name() const override;
	void set(const std::string_view property, const Any &value) override;
	[[nodiscard]] Any get(const std::string_view property) const override;
};