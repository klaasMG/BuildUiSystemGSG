#include "GMAKE_EXCEPTION.h"
#include "assert_print.h"
#include <cstdint>

GMAKE_EXCEPTION::GMAKE_EXCEPTION(bool debug_input){
    debug = debug_input;
    call_stack = {};
    warnings = {};
}

void GMAKE_EXCEPTION::error(int code, const std::optional<std::string>& helper_print) const {
    if (debug) {
        std::cerr << "call stack:" << std::endl;
        int call_num = 0;
        for (const std::string& call : call_stack) {
            std::cerr << "Function call number "
                      << call_num++ << ": " << call << std::endl;
        }
    }

    if (!call_stack.empty()) {
        std::cerr << "Last call: " << call_stack.back() << std::endl;
    } else {
        std::cerr << "Last call: <empty>" << std::endl;
    }

    if (helper_print.has_value()) {
        std::cerr << helper_print.value() << std::endl;
    }

    std::cerr << "Exit code: " << code << std::endl;
    std::exit(code);
}

void GMAKE_EXCEPTION::set_debug(bool debug_input) {
    debug = debug_input;
}


void GMAKE_EXCEPTION::add_to_call_stack(const std::string& text) {
    call_stack.push_back(text);
}

void GMAKE_EXCEPTION::warning(const std::string& text, const WarningLevel& warning_level){
    std::string warning_level_str = std::string();
    switch (warning_level){
        case WarningLevel::SECURITY:{
            warning_level_str = "SECURITY_WARNING";
            break;
        }
        case WarningLevel::WARNING:{
            warning_level_str = "WARNING";
            break;
        }
        case WarningLevel::WEAK_WARNING:{
            warning_level_str = "WEAK_WARNING";
            break;
        }
    }
    warnings.emplace(text, warning_level);
    std::cerr << warning_level_str << ": " << text << std::endl;
}

void GMAKE_EXCEPTION::warning_summary(){
    uint64_t num_warnings = 0;
    uint64_t num_security = 0;
    uint64_t num_weak_warnings = 0;
    for (const std::pair<const std::string, WarningLevel>& entry : warnings) {
        const std::string& key = entry.first;
        WarningLevel value = entry.second;
        switch (value){
            case WarningLevel::SECURITY:{
                num_security++;
                break;
            }
            case WarningLevel::WARNING:{
                num_warnings++;
                break;
            }
            case WarningLevel::WEAK_WARNING:{
                num_weak_warnings++;
                break;
            }
        }
    }
    std::cerr << "Warnings: " << num_warnings << std::endl;
    std::cerr << "Security Warnings: " << num_security << std::endl;
    std::cerr << "Weak Warnings: " << num_weak_warnings << std::endl;
}
