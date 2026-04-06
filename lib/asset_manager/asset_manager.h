#ifndef SUPERBUILD_ASSET_MANAGER_H
#define SUPERBUILD_ASSET_MANAGER_H
#include <string>
#include <filesystem>
#include <map>
#include <set>
#include <vector>
#include "Error.h"

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

    [[nodiscard]]const std::string& get() const;

private:
    FileManager* manager = nullptr;
    fs::path path;
    const std::string* data = nullptr;
};

class TextWriteHandle {
public:
    TextWriteHandle() = default;
    TextWriteHandle(FileManager* manager, fs::path path, std::string* data);

    TextWriteHandle(const TextWriteHandle&) = delete;
    TextWriteHandle& operator=(const TextWriteHandle&) = delete;

    TextWriteHandle(TextWriteHandle&& other) noexcept;
    TextWriteHandle& operator=(TextWriteHandle&& other) noexcept;

    ~TextWriteHandle();

    [[nodiscard]]std::string& get();

private:
    FileManager* manager = nullptr;
    fs::path path;
    std::string* data = nullptr;
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

    [[nodiscard]]const std::vector<uint8_t>& get() const;

private:
    FileManager* manager = nullptr;
    fs::path path;
    const std::vector<uint8_t>* data = nullptr;
};


class BinaryWriteHandle {
public:
    BinaryWriteHandle() = default;
    BinaryWriteHandle(FileManager* manager, fs::path path, std::vector<uint8_t>* data);

    BinaryWriteHandle(const BinaryWriteHandle&) = delete;
    BinaryWriteHandle& operator=(const BinaryWriteHandle&) = delete;

    BinaryWriteHandle(BinaryWriteHandle&& other) noexcept;
    BinaryWriteHandle& operator=(BinaryWriteHandle&& other) noexcept;

    ~BinaryWriteHandle();

    [[nodiscard]]std::vector<uint8_t>& get();

private:
    FileManager* manager = nullptr;
    fs::path path;
    std::vector<uint8_t>* data = nullptr;
};

class FileManager {
public:
    Result<TextHandle> reqeust_text_file(const fs::path& abs_paths);
    void return_text_file(const fs::path& abs_paths);
    Result<BinaryHandle> request_binary_file(const fs::path& path);
    void return_binary_file(const fs::path& path);
    Result<TextWriteHandle> request_text_write(const fs::path& path);
    void return_text_write(const fs::path& path);
    Result<BinaryWriteHandle> request_binary_write(const fs::path& path);
    void return_binary_write(const fs::path& path);
private:
    std::map<fs::path, uint64_t> assets_opened;
    std::map<fs::path, std::string> text_files = {{"",""}};
    std::map<fs::path, std::vector<uint8_t>> binary_files = {{"",{}}};
    std::set<fs::path> write_files = {};
};

#endif //SUPERBUILD_ASSET_MANAGER_H