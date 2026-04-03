#include "GMakeTypes.h"

GMakeFunction parseFunction(const std::string& name) {
    static const std::unordered_map<std::string, GMakeFunction> functionMap = {
        {"SetProjectDirectory", GMakeFunction::SET_PROJECT_DIRECTORY},
        {"SetProgram", GMakeFunction::SET_PROGRAM},
        {"ExtendStandard", GMakeFunction::EXTEND_STANDARD},
        {"SetLayoutBinding", GMakeFunction::SSBO_LAYOUT_BINDING}
    };

    std::unordered_map<std::string, GMakeFunction>::const_iterator it = functionMap.find(name);
    return (it != functionMap.end()) ? it->second : GMakeFunction::UNKNOWN;
}
