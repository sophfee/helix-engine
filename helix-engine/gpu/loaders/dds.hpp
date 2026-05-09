#pragma once
#include "types.hpp"

extern Error DDS_UploadFromStdIO(FILE *file, u32 texture_object, std::string &error);
