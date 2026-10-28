#include "stdx/scan.hpp"

using namespace stdx::literals;

#if defined(UNCLOSED_PLACEHOLDER)  // expected-error: Unclosed last placeholder
constexpr auto result = stdx::scan<"value {"_fs, "value 1">();
#elif defined(MISSING_CLOSING_BRACE)  // expected-error: been found in appropriate place
constexpr auto result = stdx::scan<"{%d"_fs, "1", int>();
#elif defined(INVALID_SPECIFIER)      // expected-error: Invalid specifier
constexpr auto result = stdx::scan<"{%x}"_fs, "1", int>();
#elif defined(INVALID_LITERAL)        // expected-error: Invalid specifier
constexpr auto format = "{%x}"_fs;
#elif defined(F_SPECIFIER)            // expected-error: Invalid specifier
constexpr auto result = stdx::scan<"{%f}"_fs, "1.5", int>();
#elif defined(ADJACENT_PLACEHOLDERS)  // expected-error: Placeholders in the format string must be separated by text
constexpr auto result = stdx::scan<"{}{}"_fs, "12", int, int>();
#endif
