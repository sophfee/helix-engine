	#pragma once
#include "engine/rid.hpp"

class HiZ {
public:
	HiZ();
	HiZ(HiZ const &) = delete;
	HiZ(HiZ &&) = delete;
	HiZ& operator=(HiZ const &) = delete;
	HiZ& operator=(HiZ &&) = delete;
	~HiZ();
	
	RID hi_z_shader_;
	RID hi_z_bind_group_layout_;
	RID hi_z_pipeline_layout_;
	RID hi_z_pipeline_;
	
private:
};
