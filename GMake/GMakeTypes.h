//
// Created by klaas on 2/11/2026.
//

#ifndef EVENT_STRUCT_GMAKETYPES_H
#define EVENT_STRUCT_GMAKETYPES_H
#include <filesystem>

#include "string"
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

inline GMakeFunction parseFunction(const std::string& name) {
    static const std::unordered_map<std::string, GMakeFunction> functionMap = {
        {"SetProjectDirectory", GMakeFunction::SET_PROJECT_DIRECTORY},
        {"SetProgram", GMakeFunction::SET_PROGRAM},
        {"ExtendStandard", GMakeFunction::EXTEND_STANDARD},
        {"SetLayoutBinding", GMakeFunction::SSBO_LAYOUT_BINDING}
    };

    std::unordered_map<std::string, GMakeFunction>::const_iterator it = functionMap.find(name);
    return (it != functionMap.end()) ? it->second : GMakeFunction::UNKNOWN;
}

#endif //EVENT_STRUCT_GMAKETYPES_H