#include "Error.h"

#include <iostream>

std::string ErrorType_to_string(ErrorType type) {
    switch (type) {
        case ErrorType::OK: {
            return "OK error";
        }
        case ErrorType::NOT_IMPLEMENTED: {
            return "Not implemented";
        }
        case ErrorType::FILE_NOT_FOUND: {
            return "File not found";
        }
        case ErrorType::FILE_IN_USE: {
            return "File in use";
        }
        case ErrorType::FILE_DATA_ERROR: {
            return "File data error";
        }
    }
    std::cerr << "This can not happen add all other types and this will no longer fail" << std::endl;
    std::terminate();
};

template <typename Data>
Result<Data>::Result(const Data& data) {
    this->data = data;
    type = ErrorType::OK;
}

template <typename Data>
Result<Data>::Result(const Data& data, const ErrorType& type) {
    if (type == ErrorType::OK) {
        std::cerr << "a error can not be ok" << std::endl;
        std::terminate();
    }
    this->data = data;
    this->type = type;
}

template <typename Data>
Result<Data>::~Result() {
    if (!is_error_handeled) {
        std::terminate();
    }
}

template <typename Data>
Result<Data>::Result(const ErrorType& type) {
    if (type == ErrorType::OK) {
        std::cerr << "a error can not be ok" << std::endl;
        std::terminate();
    }
    this->type = type;
    this->data = {};
}

template <typename Data>
ErrorType Result<Data>::check_error() {
    is_error_checked = true;
    return type;
}

template <typename Data>
Data Result<Data>::GetData() const {
    if (!is_error_checked) {
        std::cerr << "check error first" << std::endl;
        std::terminate();
    }
    return data;
}

template <typename Data>
Data Result<Data>::Handle_Error() {
    if (!is_error_checked) {
        std::terminate();
    }
    is_error_handeled = true;
    type = ErrorType::OK;
    return data;
}
