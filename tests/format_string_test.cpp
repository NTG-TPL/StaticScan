#include "stdx/format_string.hpp"

#include <array>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace {

using stdx::details::format_string;
using stdx::details::placeholder_position;
using namespace stdx::literals;
using namespace std::string_view_literals;

static_assert(format_string<"">::number_placeholders == 0, "number_placeholders: empty string");
static_assert(format_string<"no placeholders">::number_placeholders == 0, "number_placeholders: no placeholders");
static_assert(format_string<"{} and {%d}, {%u}, {%s}">::number_placeholders == 4,
              "number_placeholders: all kinds of placeholders");
static_assert(format_string<"a } b {}">::number_placeholders == 1, "number_placeholders: a lone '}' is plain text");

using sample = format_string<"x={}, y={%u}">;
static_assert(sample::fmt.view() == "x={}, y={%u}"sv, "format_string: fmt gives access to the template argument");
static_assert(sample::is_valid, "format_string: is_valid");
static_assert(sample::placeholder_positions == std::array{placeholder_position{2, 3}, placeholder_position{8, 11}},
              "format_string: placeholder_positions");
static_assert(format_string<"text">::placeholder_positions.empty(), "format_string: no placeholder positions");

static_assert(std::is_same_v<decltype("x={}, y={%u}"_fs), sample>, "_fs: makes a format_string");

}  // namespace
