#pragma once

#include "asset_core.hpp"

class TextAsset : public IAsset {
public:
	TextAsset();
	TextAsset(const TextAsset &) = delete;
	const TextAsset &operator=(const TextAsset &) = delete;
	TextAsset(TextAsset &&) = delete;
	TextAsset &operator=(TextAsset &&) = delete;
	~TextAsset() override = default;
	
	[[nodiscard]] String serialize() const override;
	void deserialize(StringView serialized) override;
	
	[[nodiscard]] StringView get_type() const override;
	
	[[nodiscard]] StringView get_name() const override;
	void set_name(StringView) override;
	
	[[nodiscard]] Path get_path() const override;
	void set_path(Path) override;
	
	[[nodiscard]] std::any get(StringView key) const override;
	
	void set(StringView key, std::any value) override;
	[[nodiscard]] Vector<IAsset *> get_dependencies() const override;
	
	void add_dependency(IAsset *) override;
	void remove_dependency(IAsset *) override;
	
	[[nodiscard]] Vector<StringView> get_tags() const override;
	[[nodiscard]] bool has_tag(StringView) const override;
	
	void add_tag(StringView) override;
	void remove_tag(StringView) override;
	
	[[nodiscard]] bool is_imported() const override;
	[[nodiscard]] bool import() override;
	[[nodiscard]] bool reimport() override;
	
	[[nodiscard]] IAsset * get_thumbnail() const override;
	void set_thumbnail(IAsset *) override;
	
	[[nodiscard]] bool is_valid() const override;
	
	[[nodiscard]] bool is_hidden() const override;
	void set_hidden_flag(bool) override;
	
	[[nodiscard]] bool is_internal() const override;
	void set_internal_flag(bool) override;
	
	[[nodiscard]] bool is_generated() const override;
	void set_generated_flag(bool) override;
	
	void set_text(StringView text);
	[[nodiscard]] StringView get_text() const;
	
private:
	String name_;
	String path_;
	String text_;
	Vector<IAsset *> dependencies_;
	Vector<String> tags_;
	bool imported_ = false;
	bool hidden_ = false;
	bool internal_ = false;
	bool generated_ = false;
};