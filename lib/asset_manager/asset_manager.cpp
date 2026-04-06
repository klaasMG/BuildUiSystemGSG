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

TextWriteHandle::TextWriteHandle(FileManager* manager, fs::path path, std::string* data) {
    this->manager = manager;
    this->path = std::move(path);
    this->data = data;
}

TextWriteHandle::TextWriteHandle(TextWriteHandle&& other) noexcept {
    manager = other.manager;
    path = std::move(other.path);
    data = other.data;

    other.manager = nullptr;
    other.data = nullptr;
}

TextWriteHandle& TextWriteHandle::operator=(TextWriteHandle&& other) noexcept {
    if (this != &other) {
        if (manager) {
            manager->return_text_write(path);
        }

        manager = other.manager;
        path = std::move(other.path);
        data = other.data;

        other.manager = nullptr;
        other.data = nullptr;
    }
    return *this;
}

TextWriteHandle::~TextWriteHandle() {
    std::ofstream file(path, std::ios::binary);
    file.write(data->data(), data->size());
    if (manager) {
        manager->return_text_write(path);
    }
}

std::string& TextWriteHandle::get() {
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

BinaryWriteHandle::BinaryWriteHandle(FileManager* manager, fs::path path, std::vector<uint8_t>* data) {
    this->manager = manager;
    this->path = std::move(path);
    this->data = data;
}

BinaryWriteHandle::BinaryWriteHandle(BinaryWriteHandle&& other) noexcept {
    manager = other.manager;
    path = std::move(other.path);
    data = other.data;

    other.manager = nullptr;
    other.data = nullptr;
}

BinaryWriteHandle& BinaryWriteHandle::operator=(BinaryWriteHandle&& other) noexcept {
    if (this != &other) {
        if (manager) {
            manager->return_binary_write(path);
        }

        manager = other.manager;
        path = std::move(other.path);
        data = other.data;

        other.manager = nullptr;
        other.data = nullptr;
    }
    return *this;
}

std::vector<uint8_t>& BinaryWriteHandle::get() {
    return *data;
}

BinaryWriteHandle::~BinaryWriteHandle() {
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data->data()), data->size());
    if (manager) {
        manager->return_binary_write(path);
    }
}

Result<TextHandle> FileManager::reqeust_text_file(const fs::path& paths) {
    fs::path abs_paths = fs::absolute(paths);
    if (write_files.contains(abs_paths)) {
        ErrorType error = ErrorType::FILE_IN_USE;
        Result text_result = Result<TextHandle>(error);
        return text_result;
    }
    std::string data;

    if (text_files.find(abs_paths) == text_files.end()) {
        std::ifstream file(abs_paths, std::ios::binary);

        // ✅ FIX 3: file check added
        if (!file) {
            ErrorType error = ErrorType::FILE_NOT_FOUND;
            Result text_result = Result<TextHandle>(error);
            return text_result;
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

    return  Result{TextHandle(this, abs_paths, &text_files[abs_paths])};
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

Result<BinaryHandle> FileManager::request_binary_file(const fs::path& path) {
    fs::path abs_paths = fs::absolute(path);
    if (write_files.contains(abs_paths)) {
        ErrorType error = ErrorType::FILE_IN_USE;
        Result text_result = Result<BinaryHandle>(error);
        return text_result;
    }
    std::vector<uint8_t> data;

    if (binary_files.find(abs_paths) == binary_files.end()) {
        std::ifstream file(abs_paths, std::ios::binary);

        if (!file) {
            ErrorType error = ErrorType::FILE_NOT_FOUND;
            Result text_result = Result<BinaryHandle>(error);
            return text_result;
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

    return Result{BinaryHandle(this, abs_paths, &binary_files[abs_paths])};
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

Result<TextWriteHandle> FileManager::request_text_write(const fs::path& path) {
    fs::path abs_paths = fs::absolute(path);
    if (assets_opened.contains(abs_paths) || write_files.contains(abs_paths)) {
        ErrorType error = ErrorType::FILE_IN_USE;
        Result text_result = Result<TextWriteHandle>(error);
        return text_result;
    }
    write_files.insert(abs_paths);
    std::string data;
    std::ifstream file(abs_paths, std::ios::binary);
    if (!file) {
        ErrorType error = ErrorType::FILE_NOT_FOUND;
        Result text_result = Result<TextWriteHandle>(error);
        write_files.erase(path);
        return text_result;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    std::string data1(size, '\0');
    data = std::move(data1);
    file.read(data.data(), size);

    text_files[abs_paths] = data;
    return Result{TextWriteHandle(this, abs_paths, &text_files[abs_paths])};
}

void FileManager::return_text_write(const fs::path& path) {
    fs::path abs_paths = fs::absolute(path);
    if (text_files.find(abs_paths) == text_files.end()) {
        return;
    }
    write_files.erase(abs_paths);
    text_files.erase(abs_paths);
    assets_opened.erase(abs_paths);
}

Result<BinaryWriteHandle> FileManager::request_binary_write(const fs::path& path) {
    fs::path abs_paths = fs::absolute(path);
    if (assets_opened.contains(abs_paths) || write_files.contains(abs_paths)) {
        ErrorType error = ErrorType::FILE_IN_USE;
        Result text_result = Result<BinaryWriteHandle>(error);
        
        return text_result;
    }
    write_files.insert(abs_paths);
    std::vector<uint8_t> data;
    std::ifstream file(abs_paths, std::ios::binary);

    if (!file) {
        ErrorType error = ErrorType::FILE_NOT_FOUND;
        Result text_result = Result<BinaryWriteHandle>(error);
        write_files.erase(path);
        return text_result;
    }

    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size);

    binary_files[abs_paths] = data;
    return Result{BinaryWriteHandle(this, abs_paths, &binary_files[abs_paths])};
}

void FileManager::return_binary_write(const fs::path& path) {
    fs::path abs_paths = fs::absolute(path);
    if (binary_files.find(abs_paths) == binary_files.end()) {
        return;
    }
    write_files.erase(abs_paths);
    binary_files.erase(abs_paths);
    assets_opened.erase(abs_paths);
}