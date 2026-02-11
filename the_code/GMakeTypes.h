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
    UNKNOWN
};

struct GMAKEConfig {
    fs::path ProjectDir;
    std::map<std::string, std::vector<fs::path>> ShaderPrograms;
    std::vector<fs::path> StandardExtensions;
};

inline GMakeFunction parseFunction(const std::string& name) {
    static const std::unordered_map<std::string, GMakeFunction> functionMap = {
        {"SetProjectDirectory", GMakeFunction::SET_PROJECT_DIRECTORY},
        {"SetProgram", GMakeFunction::SET_PROGRAM},
        {"ExtendStandard", GMakeFunction::EXTEND_STANDARD}
    };

    std::unordered_map<std::string, GMakeFunction>::const_iterator it = functionMap.find(name);
    return (it != functionMap.end()) ? it->second : GMakeFunction::UNKNOWN;
}

#endif //EVENT_STRUCT_GMAKETYPES_H