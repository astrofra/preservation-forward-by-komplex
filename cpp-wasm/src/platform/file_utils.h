#ifndef FORWARD_OFFLINE_PLATFORM_FILE_UTILS_H
#define FORWARD_OFFLINE_PLATFORM_FILE_UTILS_H

#include <string>

namespace forward_offline {

std::string normalize_separators(const std::string& path);
std::string join_path(const std::string& left, const std::string& right);
bool create_directories(const std::string& path, std::string* error_message);

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_PLATFORM_FILE_UTILS_H
