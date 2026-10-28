#include "stdx/scan.hpp"

#include <string_view>

using namespace stdx::literals;

int main() {
    constexpr auto result = stdx::scan<"user={%s}, id={%u}"_fs, "user=alice, id=7", std::string_view, unsigned>();
    const auto &[user, id] = result.values();
    return user == "alice" && id == 7 ? 0 : 1;
}
