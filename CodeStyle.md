# 1.**Inheritance**
1. Only from external libraries
2. Avoid it at all cost
3. If ever used only a single layer deep
# **2.Code Formating**
1. The bracket is always at the end of the line
2. Comments are to be avoided if documentation is needed in code use doc strings
3. In any loop or if statement in brackets must be in brackets
   1. Not allowed: if (treu) std::cout << "thing" << std::endl;
   2. Allowed: if (treu) { std::cout << "thing" << std::endl }
# **Functions**
1. explicit returns are mandatory if the function is not from an external library