#pragma once

#include <string>

#include "Mesh.h"

namespace forward::core {

bool LoadIguMesh(const std::string& path, Mesh& out_mesh, std::string* out_error);

}  // namespace forward::core
