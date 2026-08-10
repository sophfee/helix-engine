#pragma once
#include "types.hpp"

struct RID;
extern Error DDS_UploadFromStdIO(FILE *file, std::shared_ptr<RID> texture_rid, std::string &error);
