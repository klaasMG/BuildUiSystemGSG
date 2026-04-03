#include "file_utils.h"

namespace fs = std::filesystem;

std::string readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    std::string data(size, '\0');
    file.read(data.data(), size);

    return data;
}

void WriteFile(const fs::path& filepath, const std::string& content) {
    std::ofstream file(filepath);
    file << content;
    file.close();
}

std::string ReadFilePath(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    std::string data(size, '\0');
    file.read(data.data(), size);

    return data;
}
