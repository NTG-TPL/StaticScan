#include "stdx/scan.hpp"

using namespace stdx::literals;

#if defined(RUNTIME_CALL)  // expected-error: immediate function|consteval function
auto *const runtime_scan = &stdx::scan<"{}"_fs, "1", int>;
#endif
