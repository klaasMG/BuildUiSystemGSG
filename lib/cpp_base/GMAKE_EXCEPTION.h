#ifndef GMAKE_EXCEPTION_H
#define GMAKE_EXCEPTION_H

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <optional>
#include <map>

enum class WarningLevel{
    WARNING,
    WEAK_WARNING,
    SECURITY,
};

class GMAKE_EXCEPTION {
public:
    explicit GMAKE_EXCEPTION(bool debug_input);

    void error(int code, const std::optional<std::string>& helper_print) const;

    void set_debug(bool debug_input);

    void add_to_call_stack(const std::string& text);

    void warning(const std::string& text, const WarningLevel& warning_level);

    void warning_summary();

private:
    std::map<std::string, WarningLevel> warnings;
    std::vector<std::string> call_stack;
    bool debug = false;
};

#endif // GMAKE_EXCEPTION_H