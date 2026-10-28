#include "stdx/scan.hpp"

using namespace stdx::literals;

#if defined(D_FOR_UNSIGNED)  // expected-error: The %d specifier requires a signed integer type
constexpr auto result = stdx::scan<"{%d}"_fs, "1", unsigned>();
#elif defined(U_FOR_SIGNED)   // expected-error: The %u specifier requires an unsigned integer type
constexpr auto result = stdx::scan<"{%u}"_fs, "1", int>();
#elif defined(S_FOR_INTEGER)  // expected-error: The %s specifier requires std::string_view
constexpr auto result = stdx::scan<"{%s}"_fs, "1", int>();
#endif
