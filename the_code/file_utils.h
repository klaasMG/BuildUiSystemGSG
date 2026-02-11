//
// Created by klaas on 2/11/2026.
//

#ifndef EVENT_STRUCT_FILE_UTILS_H
#define EVENT_STRUCT_FILE_UTILS_H
#include <filesystem>
#include <string>
#include <fstream>
namespace fs = std::filesystem;

inline std::string insertLine(std::string text, int lineIndex, const std::string& newLine){
    size_t pos = 0;
    int line = 0;

    while (line < lineIndex && pos != std::string::npos)
    {
        pos = text.find('\n', pos);

        if (pos != std::string::npos)
            pos++; // move past '\n'

        line++;
    }

    if (pos == std::string::npos)
        text += newLine + "\n";   // append if too short
    else
        text.insert(pos, newLine + "\n");

    return text;
}

inline std::string readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    std::string data(size, '\0');
    file.read(data.data(), size);

    return data;
}

inline void WriteFile(const fs::path& filepath, const std::string& content) {
    std::ofstream file(filepath);
    file << content;
    file.close();  // Optional, destructor closes it
}

inline std::string ReadFilePath(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    std::string data(size, '\0');
    file.read(data.data(), size);

    return data;
}

#endif //EVENT_STRUCT_FILE_UTILS_H