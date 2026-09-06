#pragma once
#include <any>
#include <chrono>

#include "types.hpp"
#include <string_view>

// Defines several core types for the asset browser system

class ISerializable {
public:
	virtual ~ISerializable() = default;

	[[nodiscard]] virtual String serialize() const = 0;
	virtual void deserialize(StringView) = 0;
};

class IAsset : public ISerializable {
public:
	~IAsset() override = default;
	
	[[nodiscard]] virtual StringView get_type() const = 0;
	
	[[nodiscard]] virtual StringView get_name() const = 0;
	virtual void set_name(StringView) = 0;
	
	[[nodiscard]] virtual Path get_path() const = 0;
	virtual void set_path(Path) = 0;
	
	[[nodiscard]] virtual std::any get(StringView key) const = 0;
	virtual void set(StringView key, std::any value) = 0;
	
	[[nodiscard]] virtual Vector<IAsset*> get_dependencies() const = 0;
	virtual void add_dependency(IAsset*) = 0;
	virtual void remove_dependency(IAsset*) = 0;
	
	[[nodiscard]] virtual Vector<StringView> get_tags() const = 0;
	[[nodiscard]] virtual bool has_tag(StringView) const = 0;
	virtual void add_tag(StringView) = 0;
	virtual void remove_tag(StringView) = 0;
	
	[[nodiscard]] virtual bool is_imported() const = 0;
	[[nodiscard]] virtual bool import() = 0;
	[[nodiscard]] virtual bool reimport() = 0;

	[[nodiscard]] virtual IAsset* get_thumbnail() const = 0;
	virtual void set_thumbnail(IAsset*) = 0;
	
	[[nodiscard]] virtual bool is_valid() const = 0;

	/** \brief Hidden items will not be shown in the asset browser. */
	[[nodiscard]] virtual bool is_hidden() const = 0;
	virtual void set_hidden_flag(bool) = 0;

	/** \brief An internal asset refers to engine dependencies. It may also be combined with the generated flag. */
	[[nodiscard]] virtual bool is_internal() const = 0;
	virtual void set_internal_flag(bool) = 0;
	
	/** \brief A generated asset is created by the engine and almost always depends on another asset. */
	[[nodiscard]] virtual bool is_generated() const = 0;
	virtual void set_generated_flag(bool) = 0;
};

template <class TAsset> requires std::is_base_of_v<IAsset, TAsset>
class IAssetLoader {
public:
	virtual ~IAssetLoader() = default;
	[[nodiscard]] virtual Vector<StringView> get_recognized_extensions() const = 0;
	[[nodiscard]] virtual bool can_load(StringView path, TAsset* asset) const = 0;
	[[nodiscard]] virtual bool load(StringView path, TAsset* asset) = 0;
	[[nodiscard]] virtual bool reload(StringView path, TAsset* asset) = 0;
};
