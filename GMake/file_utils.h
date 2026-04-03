#ifndef EVENT_STRUCT_FILE_UTILS_H
#define EVENT_STRUCT_FILE_UTILS_H
#include <filesystem>
#include <string>
#include <fstream>
#include "string_utils.h"

namespace fs = std::filesystem;

std::string readFile(const char* path);
void WriteFile(const fs::path& filepath, const std::string& content);
std::string ReadFilePath(const fs::path& path);

#endif
