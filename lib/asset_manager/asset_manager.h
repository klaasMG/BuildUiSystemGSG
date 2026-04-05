#ifndef SUPERBUILD_ASSET_MANAGER_H
#define SUPERBUILD_ASSET_MANAGER_H
#include <string>
#include <filesystem>
#include <map>
#include <vector>

namespace fs = std::filesystem;

class FileManager;

class TextHandle {
public:
    TextHandle() = default;
    TextHandle(FileManager* manager, fs::path path, const std::string* data);

    TextHandle(const TextHandle&) = delete;
    TextHandle& operator=(const TextHandle&) = delete;

    TextHandle(TextHandle&& other) noexcept;
    TextHandle& operator=(TextHandle&& other) noexcept;

    ~TextHandle();

    const std::string& get() const;

private:
    FileManager* manager = nullptr;
    fs::path path;
    const std::string* data = nullptr;
};

class BinaryHandle {
public:
    BinaryHandle() = default;
    BinaryHandle(FileManager* manager, fs::path path, const std::vector<uint8_t>* data);

    BinaryHandle(const BinaryHandle&) = delete;
    BinaryHandle& operator=(const BinaryHandle&) = delete;

    BinaryHandle(BinaryHandle&& other) noexcept;
    BinaryHandle& operator=(BinaryHandle&& other) noexcept;

    ~BinaryHandle();

    const std::vector<uint8_t>& get() const;

private:
    FileManager* manager = nullptr;
    fs::path path;
    const std::vector<uint8_t>* data = nullptr;
};


class FileManager {
public:
    TextHandle reqeust_text_file(const fs::path& abs_paths);
    void return_text_file(const fs::path& abs_paths);
    BinaryHandle request_binary_file(const fs::path& path);
    void return_binary_file(const fs::path& path);
private:
    std::map<fs::path, uint64_t> assets_opened;
    std::map<fs::path, std::string> text_files;
    std::map<fs::path, std::vector<uint8_t>> binary_files;
};

#endif //SUPERBUILD_ASSET_MANAGER_H