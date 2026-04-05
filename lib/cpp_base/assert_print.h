//
// Created by klaas on 3/30/2026.
//

#ifndef SUPERBUILD_ASSERT_PRINT_H
#define SUPERBUILD_ASSERT_PRINT_H

#define PRINT(text) std::cout << text << std::endl
#define ASSERT_MSG(cond, msg)\
do {\
    if (!(cond)) {\
        std::cerr << "Assertion failed: " << msg << "\nFile: " << __FILE__ << "\nLine: " << __LINE__ << std::endl;\
        std::abort();\
    }\
} while (0)

#endif //SUPERBUILD_ASSERT_PRINT_H