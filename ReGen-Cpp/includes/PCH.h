#ifndef PCH_H
#define PCH_H

#include <cassert>
#include <string>
#include <memory>

#include <unordered_map>
#include <map>
#include <vector>
#include <list>

#ifndef NDEBUG
#include <iostream>
#define PRINTLN(message) std::cout << (message) << std::endl // NOLINT(cppcoreguidelines-macro-usage)
#define PRINT(message) std::cout << (message) // NOLINT(cppcoreguidelines-macro-usage)
#define PRINT_SEPARATOR() PRINTLN("=========================") // NOLINT(cppcoreguidelines-macro-usage)
#else
#define PRINTLN(message)
#define PRINT(message)
#define PRINT_SEPARATOR()
#endif


#endif // PCH_H
