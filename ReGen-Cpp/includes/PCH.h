#ifndef PCH_H
#define PCH_H

#include <cassert>
#include <string>

#ifndef NDEBUG
#include <iostream>

#define DBG(message) std::cout << (message) << std::endl // NOLINT(cppcoreguidelines-macro-usage)
#else
#define DBG(message)
#endif


#endif // PCH_H
