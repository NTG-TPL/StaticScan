#include <string>

#include "stdx/scan.hpp"

using namespace stdx::literals;

#if defined(UNSUPPORTED_TYPE)  // expected-error: Supported types are int8_t
constexpr auto result = stdx::scan<"{}"_fs, "1", bool>();
#elif defined(STD_STRING)      // expected-error: Supported types are int8_t
constexpr auto result = stdx::scan<"{}"_fs, "1", std::string>();
#elif defined(REFERENCE_TYPE)  // expected-error: Supported types are int8_t
constexpr auto result = stdx::scan<"{}"_fs, "1", const int &>();
#elif defined(COUNT_MISMATCH)  // expected-error: number of placeholders in the format string differs
constexpr auto result = stdx::scan<"{} {}"_fs, "1 2", int>();
#endif
