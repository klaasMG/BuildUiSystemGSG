//
// Created by klaas on 2/11/2026.
//

#ifndef EVENT_STRUCT_GMAKE_EXEPTION_H
#define EVENT_STRUCT_GMAKE_EXEPTION_H
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

class GMAKE_EXCEPTION{
private:
    std::vector<std::string> call_stack;
    bool debug = false;
public:
    GMAKE_EXCEPTION(bool debug_input){
        debug = debug_input;
        call_stack = std::vector<std::string>();
    }
    void send(const int code, const std::optional<std::string>& helper_print) const {
        if (debug){
            std::cout << "call stack: " << std::endl;
            int call_num = 0;
            for (const std::string &call: call_stack){
                std::cout << "Function call number" << std::to_string(call_num) << ":" << call << std::endl;
            }
        }
        std::string last_call = call_stack.back();
        std::cout << "Last call:" << last_call << std::endl;
        if (helper_print.has_value()){
            std::cout << helper_print.value() << std::endl;
        }
        std::cerr << "Exit code:" << std::to_string(code) << std::endl;
        std::exit(code);
    }

    void add_to_call_stack(const std::string& text){
        call_stack.push_back(text);
    }
};

#endif //EVENT_STRUCT_GMAKE_EXEPTION_H