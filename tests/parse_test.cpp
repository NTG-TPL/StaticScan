#include "stdx/parse.hpp"

#include <cstdint>
#include <limits>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace {

using stdx::details::get_current_source_for_parsing;
using stdx::details::get_specifier;
using stdx::details::parse_input;
using stdx::details::parse_value;
using stdx::details::to_integer;
using namespace stdx::literals;
using namespace std::string_view_literals;
namespace specifier = stdx::details::specifier;

using bounds = stdx::details::value_bounds;

constexpr auto format = "a={}, b={%d}."_fs;
static_assert(get_current_source_for_parsing<0, format, "a=12, b=-3.">() == bounds{2, 4},
              "get_current_source_for_parsing: first placeholder");
static_assert(get_current_source_for_parsing<1, format, "a=12, b=-3.">() == bounds{8, 10},
              "get_current_source_for_parsing: second placeholder");

static_assert(get_current_source_for_parsing<1, "{}-{}"_fs, "10-20">() == bounds{3, 5},
              "get_current_source_for_parsing: value after a placeholder at the start");
static_assert(get_current_source_for_parsing<1, "{} {}"_fs, "1 2">() == bounds{2, 3},
              "get_current_source_for_parsing: value after a one-character separator");
static_assert(get_current_source_for_parsing<0, "x={}"_fs, "x=42">() == bounds{2, 4},
              "get_current_source_for_parsing: placeholder at the end takes the rest");
static_assert(get_current_source_for_parsing<0, "[{}]"_fs, "[]">() == bounds{1, 1},
              "get_current_source_for_parsing: empty value");
static_assert(get_current_source_for_parsing<1, "{}-{}"_fs, "a-b-c">() == bounds{2, 5},
              "get_current_source_for_parsing: the first occurrence of the separator ends a value");

static_assert(get_specifier<0, format>() == specifier::none &&
                  get_specifier<1, format>() == specifier::signed_integer::letter,
              "get_specifier");

static_assert(to_integer<int>("42") == 42 && to_integer<int>("-42") == -42, "to_integer: valid numbers");
static_assert(to_integer<int>("4x").error() == std::errc::invalid_argument, "to_integer: trailing characters");
static_assert(to_integer<int>(" 4").error() == std::errc::invalid_argument, "to_integer: leading space");
static_assert(to_integer<int>("+4").error() == std::errc::invalid_argument, "to_integer: plus sign");
static_assert(to_integer<int>("").error() == std::errc::invalid_argument, "to_integer: empty string");
static_assert(to_integer<std::int8_t>("128").error() == std::errc::result_out_of_range, "to_integer: overflow");
static_assert(to_integer<std::uint32_t>("-1").error() == std::errc::invalid_argument, "to_integer: negative unsigned");

static_assert(parse_value<std::int16_t, "-15">() == -15, "parse_value: signed integer");
static_assert(parse_value<std::uint64_t, "18446744073709551615">() == std::numeric_limits<std::uint64_t>::max(),
              "parse_value: unsigned integer");
static_assert(parse_value<std::string_view, "some text">() == "some text"sv, "parse_value: std::string_view");

static_assert(parse_input<1, format, "a=12, b=-3.", const std::int32_t>() == -3, "parse_input: value");
static_assert(std::is_same_v<decltype(parse_input<1, format, "a=12, b=-3.", const std::int32_t>()), std::int32_t>,
              "parse_input: drops cv-qualifiers");

}  // namespace
