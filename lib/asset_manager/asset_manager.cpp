#include "asset_manager.h"
#include <fstream>

TextHandle::TextHandle(FileManager* manager, fs::path path, const std::string* data)
    : manager(manager), path(std::move(path)), data(data) {}

TextHandle::~TextHandle() {
    if (manager) {
        manager->return_text_file(path);
    }
}

TextHandle::TextHandle(TextHandle&& other) noexcept {
    manager = other.manager;
    path = std::move(other.path);
    data = other.data;

    other.manager = nullptr;
    other.data = nullptr;
}

TextHandle& TextHandle::operator=(TextHandle&& other) noexcept {
    if (this != &other) {
        if (manager) {
            manager->return_text_file(path);
        }

        manager = other.manager;
        path = std::move(other.path);
        data = other.data;

        other.manager = nullptr;
        other.data = nullptr;
    }
    return *this;
}

const std::string& TextHandle::get() const {
    return *data;
}

//
// BinaryHandle
//
BinaryHandle::BinaryHandle(FileManager* manager, fs::path path, const std::vector<uint8_t>* data)
    : manager(manager), path(std::move(path)), data(data) {}

BinaryHandle::~BinaryHandle() {
    if (manager) {
        manager->return_binary_file(path);
    }
}

BinaryHandle::BinaryHandle(BinaryHandle&& other) noexcept {
    manager = other.manager;
    path = std::move(other.path);
    data = other.data;

    other.manager = nullptr;
    other.data = nullptr;
}

BinaryHandle& BinaryHandle::operator=(BinaryHandle&& other) noexcept {
    if (this != &other) {
        if (manager) {
            manager->return_binary_file(path);
        }

        manager = other.manager;
        path = std::move(other.path);
        data = other.data;

        other.manager = nullptr;
        other.data = nullptr;
    }
    return *this;
}

const std::vector<uint8_t>& BinaryHandle::get() const {
    return *data;
}


TextHandle FileManager::reqeust_text_file(const fs::path& paths) {
    fs::path abs_paths = fs::absolute(paths);
    std::string data;

    if (text_files.find(abs_paths) == text_files.end()) {
        std::ifstream file(abs_paths, std::ios::binary);

        // ✅ FIX 3: file check added
        if (!file) {
            return {};
        }

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0);

        std::string data1(size, '\0');
        data = std::move(data1);
        file.read(data.data(), size);
    } else {
        data = text_files.at(abs_paths);
    }

    uint64_t num_times = 1;

    if (assets_opened.contains(abs_paths)) {
        num_times = assets_opened.at(abs_paths);
        num_times = num_times + 1;
    }

    // ✅ FIX 1: replace insert with assignment
    assets_opened[abs_paths] = num_times;

    // ✅ FIX 1: replace insert with assignment
    text_files[abs_paths] = data;

    return TextHandle(this, abs_paths, &text_files[abs_paths]);
}

void FileManager::return_text_file(const fs::path& paths) {
    fs::path abs_paths = fs::absolute(paths);
    if (text_files.find(abs_paths) == text_files.end()) {
        return;
    }

    uint64_t num_opened = assets_opened.at(abs_paths);

    if (num_opened == 1) {
        text_files.erase(abs_paths);
        assets_opened.erase(abs_paths);
    } else {
        num_opened = num_opened - 1;

        // ✅ FIX 1: assignment instead of insert
        assets_opened[abs_paths] = num_opened;
    }
}

BinaryHandle FileManager::request_binary_file(const fs::path& path) {
    fs::path abs_paths = fs::absolute(path);
    std::vector<uint8_t> data;

    if (binary_files.find(abs_paths) == binary_files.end()) {
        std::ifstream file(abs_paths, std::ios::binary);

        if (!file) {
            return {};
        }

        file.seekg(0, std::ios::end);
        size_t size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
    } else {
        data = binary_files.at(abs_paths);
    }

    uint64_t num_times = 1;

    if (assets_opened.contains(abs_paths)) {
        num_times = assets_opened.at(abs_paths) + 1;
    }

    // already correct here, keep
    assets_opened[abs_paths] = num_times;
    binary_files[abs_paths] = data;

    return BinaryHandle(this, abs_paths, &binary_files[abs_paths]);
}

void FileManager::return_binary_file(const fs::path& path) {
    fs::path abs_paths = fs::absolute(path);
    if (binary_files.find(abs_paths) == binary_files.end()) {
        return;
    }

    uint64_t num_opened = assets_opened.at(abs_paths);

    if (num_opened <= 1) {
        binary_files.erase(abs_paths);
        assets_opened.erase(abs_paths);
    } else {
        assets_opened[abs_paths] = num_opened - 1;
    }
}