#ifndef EVENT_STRUCT_GMAKETYPES_H
#define EVENT_STRUCT_GMAKETYPES_H
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace fs = std::filesystem;

enum class GMakeFunction {
    SET_PROJECT_DIRECTORY,
    SET_PROGRAM,
    EXTEND_STANDARD,
    SSBO_LAYOUT_BINDING,
    UNKNOWN
};

struct GMAKEConfig {
    bool debug = false;
    fs::path ProjectDir;
    std::map<std::string, std::vector<fs::path>> ShaderPrograms;
    std::vector<fs::path> StandardExtensions;
    std::map<std::string, std::map<std::string, uint64_t>> SSBO_key_to_value = {};
};

GMakeFunction parseFunction(const std::string& name);

#endif
