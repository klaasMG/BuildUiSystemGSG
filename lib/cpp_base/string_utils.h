//
// Created by klaas on 4/2/2026.
//

#ifndef SUPERBUILD_STRING_UTILS_H
#define SUPERBUILD_STRING_UTILS_H

#include <string>
#include <utility>

std::string toLower(std::string s);
std::string insertLine(std::string text, int lineIndex, const std::string& newLine);
std::string trim(std::string s);
std::pair<std::string, std::string> split_once(const std::string& s, char delim);
std::string replace_first(const std::string& input, const std::string& sub, const std::string& repl);
char nextNonSpace(const std::string& s, size_t pos);

#endif //SUPERBUILD_STRING_UTILS_H
