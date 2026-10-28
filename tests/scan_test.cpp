#include "stdx/scan.hpp"

#include <cstdint>
#include <limits>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace {

using namespace stdx::literals;
using namespace std::string_view_literals;

template <typename T>
constexpr T min_of = std::numeric_limits<T>::min();

template <typename T>
constexpr T max_of = std::numeric_limits<T>::max();

static_assert(stdx::scan<"I want to sum {} and {%d} numbers."_fs, "I want to sum 42 and -7 numbers.", int, int>()
                      .values() == std::tuple{42, -7},
              "scan: example from the assignment");

static_assert(stdx::scan<"{} {} {} {}"_fs, "-128 -32768 -2147483648 -9223372036854775808", std::int8_t, std::int16_t,
                         std::int32_t, std::int64_t>()
                      .values() ==
                  std::tuple{min_of<std::int8_t>, min_of<std::int16_t>, min_of<std::int32_t>, min_of<std::int64_t>},
              "scan: minimal values of signed types");

static_assert(stdx::scan<"{%u} {%u} {%u} {%u}"_fs, "255 65535 4294967295 18446744073709551615", std::uint8_t,
                         std::uint16_t, std::uint32_t, std::uint64_t>()
                      .values() ==
                  std::tuple{max_of<std::uint8_t>, max_of<std::uint16_t>, max_of<std::uint32_t>, max_of<std::uint64_t>},
              "scan: maximal values of unsigned types");

static_assert(stdx::scan<"name={%s}, city={}"_fs, "name=Bob, city=Moscow", std::string_view, std::string_view>()
                      .values() == std::tuple{"Bob"sv, "Moscow"sv},
              "scan: strings with and without the %s specifier");

static_assert(stdx::scan<"id={%u}, delta={%d}, tag={%s}"_fs, "id=7, delta=-3, tag=new", std::uint32_t, std::int32_t,
                         std::string_view>()
                      .values() == std::tuple{7U, -3, "new"sv},
              "scan: all specifiers in one format string");

static_assert(std::is_same_v<decltype(stdx::scan<"{} {} {}"_fs, "1 2 x", const std::int32_t, volatile std::uint16_t,
                                                 const volatile std::string_view>()
                                          .values()),
                             std::tuple<std::int32_t, std::uint16_t, std::string_view>>,
              "scan: cv-qualified types give values without cv-qualifiers");

static_assert(
    stdx::scan<"{} {} {}"_fs, "1 2 x", const std::int32_t, volatile std::uint16_t, const volatile std::string_view>()
            .values() == std::tuple{1, std::uint16_t{2}, "x"sv},
    "scan: values of cv-qualified types");

static_assert(stdx::scan<"{}"_fs, "-42", std::int64_t>().values() == std::tuple{std::int64_t{-42}},
              "scan: the whole source is one value");

static_assert(stdx::scan<"{}-{}"_fs, "10-20", int, int>().values() == std::tuple{10, 20},
              "scan: values at the very start and end");

static_assert(stdx::scan<"{},{},{}"_fs, "2026,10,Moscow", int, int, std::string_view>().values() ==
                  std::tuple{2026, 10, "Moscow"sv},
              "scan: CSV line");

static_assert(stdx::scan<"{} | {}"_fs, "a b | c d", std::string_view, std::string_view>().values() ==
                  std::tuple{"a b"sv, "c d"sv},
              "scan: strings with spaces and a multi-character separator");

static_assert(stdx::scan<"[{}]"_fs, "[]", std::string_view>().values() == std::tuple{""sv}, "scan: empty string value");

static_assert(stdx::scan<"{} {}"_fs, "007 -0", int, int>().values() == std::tuple{7, 0},
              "scan: leading zeros and negative zero");

static_assert(stdx::scan<"<{%s}>"_fs, "<{x}>", std::string_view>().values() == std::tuple{"{x}"sv},
              "scan: braces inside a string value");

static_assert(stdx::scan<"имя={}, возраст={%u}"_fs, "имя=Ольга, возраст=30", std::string_view, unsigned>().values() ==
                  std::tuple{"Ольга"sv, 30U},
              "scan: UTF-8 text");

static_assert(stdx::scan<"plain text"_fs, "plain text">().values() == std::tuple{},
              "scan: format string without placeholders");

static_assert(stdx::scan<""_fs, "">().values() == std::tuple{}, "scan: empty strings");

static_assert(std::get<0>(std::move(stdx::scan<"{}"_fs, "abc", std::string_view>()).values()) == "abc"sv,
              "scan: values of a temporary result");

using stdx::details::first_unmet;
using stdx::details::scan_requirements;

static_assert(!first_unmet(scan_requirements<"{} {}"_fs, "1 2", int, int>()), "first_unmet: all requirements are met");
static_assert(*first_unmet(scan_requirements<"{} {}"_fs, "1 2", int>()) ==
                  "The number of placeholders in the format string differs from the number of types"sv,
              "first_unmet: the error of the unmet requirement");
static_assert(first_unmet(scan_requirements<"{}{}"_fs, "12", bool, int>())->starts_with("Supported types"),
              "first_unmet: the first unmet requirement wins");

}  // namespace
