#include <cstdint>

#include "stdx/scan.hpp"

using namespace stdx::literals;

#if defined(NOT_A_NUMBER)  // expected-error: The value is not a valid integer
constexpr auto result = stdx::scan<"{}"_fs, "abc", int>();
#elif defined(EMPTY_NUMBER)       // expected-error: The value is not a valid integer
constexpr auto result = stdx::scan<"[{}]"_fs, "[]", int>();
#elif defined(NEGATIVE_UNSIGNED)  // expected-error: The value is not a valid integer
constexpr auto result = stdx::scan<"{%u}"_fs, "-1", std::uint32_t>();
#elif defined(OUT_OF_RANGE)       // expected-error: The value is out of range of the target type
constexpr auto result = stdx::scan<"{}"_fs, "300", std::int8_t>();
#endif
