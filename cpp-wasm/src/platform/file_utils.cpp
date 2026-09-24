#include "platform/file_utils.h"

#include <cerrno>
#include <cctype>
#include <cstring>
#include <string>
#include <sys/stat.h>

#if defined(_WIN32)
#include <direct.h>
#endif

namespace forward_offline {

namespace {

std::string last_error_string() {
#if defined(_WIN32)
    char buffer[256] = {0};
    strerror_s(buffer, sizeof(buffer), errno);
    return std::string(buffer);
#else
    return std::string(std::strerror(errno));
#endif
}

bool directory_exists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }

    return (info.st_mode & S_IFDIR) != 0;
}

bool create_single_directory(const std::string& path) {
#if defined(_WIN32)
    if (_mkdir(path.c_str()) == 0) {
        return true;
    }
#else
    if (mkdir(path.c_str(), 0755) == 0) {
        return true;
    }
#endif

    return errno == EEXIST && directory_exists(path);
}

}  // namespace

std::string normalize_separators(const std::string& path) {
    std::string normalized(path);
    for (std::size_t index = 0; index < normalized.size(); ++index) {
        if (normalized[index] == '\\') {
            normalized[index] = '/';
        }
    }
    return normalized;
}

std::string join_path(const std::string& left, const std::string& right) {
    if (left.empty()) {
        return normalize_separators(right);
    }
    if (right.empty()) {
        return normalize_separators(left);
    }

    const std::string normalized_left = normalize_separators(left);
    const std::string normalized_right = normalize_separators(right);

    if (normalized_left[normalized_left.size() - 1] == '/') {
        return normalized_left + normalized_right;
    }

    return normalized_left + "/" + normalized_right;
}

bool create_directories(const std::string& path, std::string* error_message) {
    if (path.empty()) {
        if (error_message != NULL) {
            *error_message = "empty directory path";
        }
        return false;
    }

    const std::string normalized = normalize_separators(path);
    if (directory_exists(normalized)) {
        return true;
    }

    std::string current;
    std::size_t index = 0;

    if (normalized.size() >= 2 &&
        std::isalpha(static_cast<unsigned char>(normalized[0])) &&
        normalized[1] == ':') {
        current = normalized.substr(0, 2);
        index = 2;
        if (normalized.size() > 2 && normalized[2] == '/') {
            current += '/';
            index = 3;
        }
    } else if (!normalized.empty() && normalized[0] == '/') {
        current = "/";
        index = 1;
    }

    while (index <= normalized.size()) {
        const std::size_t next = normalized.find('/', index);
        const std::string part = normalized.substr(
            index,
            next == std::string::npos ? std::string::npos : next - index);

        if (!part.empty()) {
            if (!current.empty() && current[current.size() - 1] != '/') {
                current += '/';
            }
            current += part;

            if (!directory_exists(current) && !create_single_directory(current)) {
                if (error_message != NULL) {
                    *error_message = "unable to create directory " + current + ": " +
                                     last_error_string();
                }
                return false;
            }
        }

        if (next == std::string::npos) {
            break;
        }
        index = next + 1;
    }

    return true;
}

}  // namespace forward_offline
