#include "MeshLoaderIgu.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>

namespace forward::core {
namespace {

enum class ParseBlock {
  kNone,
  kPositions,
  kTexcoords,
  kFaces,
};

std::vector<float> ExtractFloats(const std::string& line) {
  std::vector<float> values;
  const char* p = line.c_str();
  while (*p != '\0') {
    errno = 0;
    char* end = nullptr;
    const float value = std::strtof(p, &end);
    if (end != p) {
      if (errno == 0) {
        values.push_back(value);
      }
      p = end;
      continue;
    }
    ++p;
  }
  return values;
}

std::vector<int> ExtractInts(const std::string& line) {
  std::vector<int> values;
  const char* p = line.c_str();
  while (*p != '\0') {
    errno = 0;
    char* end = nullptr;
    const long value = std::strtol(p, &end, 0);
    if (end != p) {
      if (errno == 0) {
        values.push_back(static_cast<int>(value));
      }
      p = end;
      continue;
    }
    ++p;
  }
  return values;
}

std::string MakeError(const std::string& path, int line_no, const std::string& message) {
  std::ostringstream out;
  out << path << ":" << line_no << ": " << message;
  return out.str();
}

}  // namespace

bool LoadIguMesh(const std::string& path, Mesh& out_mesh, std::string* out_error) {
  std::ifstream input(path);
  if (!input.is_open()) {
    if (out_error) {
      *out_error = "unable to open file: " + path;
    }
    return false;
  }

  out_mesh.Clear();

  ParseBlock block = ParseBlock::kNone;
  int lines_remaining = 0;
  int vertex_block_count = 0;
  int line_no = 0;

  std::string line;
  while (std::getline(input, line)) {
    ++line_no;

    if (lines_remaining > 0) {
      switch (block) {
        case ParseBlock::kPositions: {
          const std::vector<float> values = ExtractFloats(line);
          if (values.size() < 3) {
            if (out_error) {
              *out_error = MakeError(path, line_no, "invalid vertex line");
            }
            return false;
          }
          out_mesh.positions.emplace_back(values[0], values[1], values[2]);
          break;
        }
        case ParseBlock::kTexcoords: {
          const std::vector<float> values = ExtractFloats(line);
          if (values.size() < 2) {
            if (out_error) {
              *out_error = MakeError(path, line_no, "invalid texcoord line");
            }
            return false;
          }
          out_mesh.texcoords.emplace_back(values[0], values[1]);
          break;
        }
        case ParseBlock::kFaces: {
          const std::vector<int> values = ExtractInts(line);
          if (values.size() < 3) {
            if (out_error) {
              *out_error = MakeError(path, line_no, "invalid face line");
            }
            return false;
          }
          out_mesh.triangles.push_back(Triangle{values[0], values[1], values[2]});
          break;
        }
        case ParseBlock::kNone:
          break;
      }

      --lines_remaining;
      if (lines_remaining == 0) {
        block = ParseBlock::kNone;
      }
      continue;
    }

    if (line.find("Vertices:") != std::string::npos) {
      const std::vector<int> values = ExtractInts(line);
      if (values.empty()) {
        continue;
      }
      lines_remaining = values.front();
      ++vertex_block_count;
      block = (vertex_block_count == 1) ? ParseBlock::kPositions : ParseBlock::kTexcoords;
      if (block == ParseBlock::kPositions) {
        out_mesh.positions.reserve(static_cast<size_t>(lines_remaining));
      } else {
        out_mesh.texcoords.reserve(static_cast<size_t>(lines_remaining));
      }
      continue;
    }

    if (line.find("Faces:") != std::string::npos) {
      const std::vector<int> values = ExtractInts(line);
      if (values.empty()) {
        continue;
      }
      lines_remaining = values.front();
      block = ParseBlock::kFaces;
      out_mesh.triangles.reserve(static_cast<size_t>(lines_remaining));
      continue;
    }
  }

  if (out_mesh.Empty()) {
    if (out_error) {
      *out_error = "mesh is empty after parse: " + path;
    }
    return false;
  }

  for (const Triangle& t : out_mesh.triangles) {
    const int vertex_count = static_cast<int>(out_mesh.positions.size());
    if (t.a < 0 || t.b < 0 || t.c < 0 || t.a >= vertex_count || t.b >= vertex_count ||
        t.c >= vertex_count) {
      if (out_error) {
        *out_error = "face index out of range in: " + path;
      }
      return false;
    }
  }

  return true;
}

}  // namespace forward::core
