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
4. between any operator like the + or = and it suroundings is one and exactly a single space
# **Functions**
1. explicit returns are mandatory if the function is not from an external library
2. in the python part use print_wrapper over print
# **naming**
1. a class is always named in Pascal case while the rest is named in snake_case
2. enums should be SCREAMING_SNAKE
# **headers**
1. use pragma once
# **error handling**
1. use the Result type for error handling it is not yet in python