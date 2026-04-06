#ifndef SUPERBUILD_ERROR_H
#define SUPERBUILD_ERROR_H
#include <optional>

enum class ErrorType {
    OK,
    NOT_IMPLEMENTED,
    FILE_NOT_FOUND,
    FILE_IN_USE,
};

template<typename Data>
class [[nodiscard]] Result {
public:
    Result() = delete;
    Result(const Result&) = delete;
    Result& operator=(const Result&) = delete;
    Result(Result&&) = default;
    Result& operator=(Result&&) = default;
    Result(const Data& data);
    Result(const Data& data, const ErrorType& type);
    Result(const ErrorType& type);
    [[nodiscard]] ErrorType check_error();
    Data GetData() const;
    Data Handle_Error();
    ~Result();
private:
    bool is_error_handeled = false;
    bool is_error_checked = false;
    ErrorType type;
    Data data;
};

#endif //SUPERBUILD_ERROR_H