#include "stdx/match.hpp"

#include <array>

namespace {

using stdx::details::all_value_bounds;
using stdx::details::placeholders_are_separated;
using stdx::details::source_matches_format;
using stdx::details::value_bounds_of;
using namespace stdx::literals;

using bounds = stdx::details::value_bounds;

constexpr auto format = "a={}, b={%d}."_fs;

static_assert(all_value_bounds<format, "a=12, b=-3.">() == std::array{bounds{2, 4}, bounds{8, 10}},
              "all_value_bounds: text around placeholders");
static_assert(all_value_bounds<"{} - {} - {}"_fs, "1 - 2 - 3">() ==
                  std::array{bounds{0, 1}, bounds{4, 5}, bounds{8, 9}},
              "all_value_bounds: three placeholders");
static_assert(all_value_bounds<"{}, {}"_fs, "1 2">() == std::array{bounds{0, 3}, bounds{3, 3}},
              "all_value_bounds: a missing separator gives the rest to the value");
static_assert(all_value_bounds<"text"_fs, "text">().empty(), "all_value_bounds: no placeholders");
static_assert(value_bounds_of<format, "a=12, b=-3."> == all_value_bounds<format, "a=12, b=-3.">(),
              "value_bounds_of: caches all_value_bounds");

static_assert(source_matches_format<format, "a=12, b=-3.">(), "source_matches_format: matching strings");
static_assert(source_matches_format<"plain"_fs, "plain">(), "source_matches_format: no placeholders");
static_assert(!source_matches_format<"plain"_fs, "other">(), "source_matches_format: different plain text");
static_assert(!source_matches_format<format, "x=12, b=-3.">(), "source_matches_format: different leading text");
static_assert(!source_matches_format<format, "a=12; b=-3.">(), "source_matches_format: missing separator");
static_assert(!source_matches_format<format, "a=12, b=-3!">(), "source_matches_format: different trailing text");
static_assert(!source_matches_format<format, "a=12, b=-3.x">(), "source_matches_format: extra trailing text");
static_assert(!source_matches_format<"long prefix {}"_fs, "1">(), "source_matches_format: source too short");

static_assert(placeholders_are_separated<format>(), "placeholders_are_separated: separated");
static_assert(!placeholders_are_separated<"{}{}"_fs>(), "placeholders_are_separated: adjacent");

}  // namespace
