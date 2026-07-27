#pragma once
#include "types.hpp"

class Texture;
extern Error DDS_UploadFromStdIO(FILE *file, std::shared_ptr<Texture> texture_object, std::string &error);
