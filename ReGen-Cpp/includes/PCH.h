#ifndef PCH_H
#define PCH_H

#include <cassert>
#include <string>

#ifndef NDEBUG
#include <iostream>
#define PRINTLN(message) std::cout << (message) << std::endl // NOLINT(cppcoreguidelines-macro-usage)
#define PRINT(message) std::cout << (message) // NOLINT(cppcoreguidelines-macro-usage)
#else
#define PRINTLN(message)
#define PRINT(message)
#endif


#endif // PCH_H
