#pragma once

#include "types.hpp"
#include "pass.hpp"

class IRenderer;

class ContactShadows : public IGpuPass {
public:
	ContactShadows();
	ContactShadows(ContactShadows const &) = delete;
	ContactShadows& operator=(ContactShadows const &) = delete;
	ContactShadows(ContactShadows &&) = delete;
	ContactShadows& operator=(ContactShadows &&) = delete;
	~ContactShadows();
	
	void record(IRenderer* renderer, RID command, Optional<RID> surface) override;
	
	RID shader_;
	RID bind_group_layout_;
	RID pipeline_layout_;
	RID pipeline_;
};
