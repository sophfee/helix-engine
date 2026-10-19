#pragma once

#include "scene_asset.hpp"

class GltfDocument : public SceneAsset {
public:
	using SelfType = GltfDocument;

	[[nodiscard]] StringView get_class_name() const override;
	void set(const std::string_view property, const Any &value) override;
	[[nodiscard]] Any get(const std::string_view property) const override;
};

class 