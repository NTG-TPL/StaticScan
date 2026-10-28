#include "stdx/scan.hpp"

using namespace stdx::literals;

#if defined(DIFFERENT_TEXT)  // expected-error: text outside the placeholders differs
constexpr auto result = stdx::scan<"x={}"_fs, "y=1", int>();
#elif defined(MISSING_SEPARATOR)    // expected-error: text outside the placeholders differs
constexpr auto result = stdx::scan<"{}, {}"_fs, "1 2", int, int>();
#elif defined(EXTRA_TRAILING_TEXT)  // expected-error: text outside the placeholders differs
constexpr auto result = stdx::scan<"{}."_fs, "1.x", int>();
#endif
